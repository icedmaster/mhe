#include "debug/rdbg.hpp"

#include "core/types_cast.hpp"
#include "utils/strutils.hpp"
#include "utils/global_log.hpp"

#include "game/engine.hpp"

#include "debug/profiler.hpp"

namespace mhe {

namespace detail {
	const uint16_t rdbg_server_port = 11111;
	const size_t max_buffer_length = 1024;
}

namespace {
const char* error_invalid_command = "Invalid command";
const char* error_unknown_command = "Unknown command";
const char* error_invalid_argument = "Invalid argument";

const char* message_ok = "ok";

const char* set_command = "s";
const char* get_command = "g";
const char* set_var_command = "sv";
const char* get_var_command = "gv";

const char* get_all_command = "get";
const char* profiler_result_command = "profiler";
const char* stats_command = "stats";

template <class T>
T convert(const std::string* args, size_t argc)
{
	ASSERT(0, "Invalid type");
	return T();
}

template <>
int convert(const std::string* args, size_t argc)
{
	ASSERT(argc == 1, "Invalid argc:" << argc);
	return types_cast<int>(args[0]);
}

template <>
vec4 convert(const std::string* args, size_t argc)
{
	ASSERT(argc == 4, "Invalid argc:" << argc);
	return vec4(types_cast<float>(args[0]), types_cast<float>(args[1]),
		types_cast<float>(args[2]), types_cast<float>(args[3]));
}

uint32_t construct(const char* data)
{
    return ((data[0] << 24) & 0xff000000) | ((data[1] << 16) & 0xff0000) | ((data[2] << 8) & 0xff00) | (data[3] & 0xff);
}

void split(uint32_t value, char* buffer)
{
	buffer[0] = (value >> 24) & 0xff;
	buffer[1] = (value >> 16) & 0xff;
	buffer[2] = (value >> 8) & 0xff;
	buffer[3] = value & 0xff;
}

inline void add_field(std::string& dst, const char* field)
{
	dst += field;
	dst += " ";
}

inline void add_field(std::string& dst, const std::string& field)
{
	add_field(dst, field.c_str());
}

inline void add_field(std::string& dst, const std::string& field, int type)
{
	dst += field;
	dst += " ";
	dst += types_cast<std::string>(type);
	dst += " ";
}

template <class T>
inline void add_stat_field(std::string& dst, const char* name, const T& value)
{
	dst += name;
	dst += ":" + types_cast<std::string>(value);
	dst += "\n";
}
}

std::string RDBGProcessor::process_command(const std::string& cmd)
{
	const std::vector<std::string>& args = utils::split(cmd, " ");
	if (args.empty())
		return make_error(error_invalid_command);
	
	std::string result;
	if (process_default_command(result, args))
		return result;

	if (process_var(result, args))
		return result;

	const std::vector<std::string>& subtypes = utils::split(args[0], ".");
	const std::string& type_name = subtypes[0];
	DataMap::iterator it = data_.find(hash(type_name));
	if (it == data_.end())
		return make_error(error_unknown_command);
	size_t id = types_cast<size_t>(args[1]);
	// The third argument should be whether we get or set data
	if (args[2] == set_command)
		return set_data(it->second, id, subtypes, &args[3], args.size() - 3);
	else if (args[2] == get_command)
		return get_data(it->second, id, subtypes);
	return make_error(error_invalid_argument);
}

bool RDBGProcessor::process_default_command(std::string& result, const std::vector<std::string>& args)
{
	const std::string& cmd = args[0];
	result.clear();
	if (cmd == get_all_command)
		result = process_get_all_command(args);
	else if (cmd == profiler_result_command)
		result = process_profiler_result_command(args);
	else if (cmd == stats_command)
		result = process_stats_command(args);
	return !result.empty();
}

bool RDBGProcessor::process_var(std::string& result, const std::vector<std::string>& args)
{
	if (args.size() < 2) return false;
	const std::string& cmd = args[0];
	const std::string& varname = args[1];

	if (!GlobalVars::instance().has(varname.c_str()))
		return false;
	if (cmd == set_var_command)
		return set_var(result, args);
	else if (cmd == get_var_command)
		return get_var(result);
	else
	{
		result = make_error(error_unknown_command);
		return true;
	}
}

bool RDBGProcessor::set_var(std::string& result, const std::vector<std::string>& args)
{
	GlobalVars::instance().set(args[1].c_str(), &args[2], args.size() - 2);
	result = "ok";
	return true;
}

bool RDBGProcessor::get_var(std::string& result)
{
	result = make_error(error_unknown_command);
	return true;
}

std::string RDBGProcessor::process_get_all_command(const std::vector<std::string>& /*args*/)
{
	std::string result;
	const std::vector<GlobalVars::Data>& global_vars = GlobalVars::instance().data();
	for (size_t i = 0; i < global_vars.size(); ++i)
	{
		add_field(result, "var");
		add_field(result, global_vars[i].name, global_vars[i].type);
	}
	return result;
}

std::string RDBGProcessor::process_profiler_result_command(const std::vector<std::string>& /*args*/)
{
	return profiler_data_;
}

std::string RDBGProcessor::process_stats_command(const std::vector<std::string>& /*args*/)
{
	std::string result;
	Driver::Stats& stats = engine_.context().driver.stats();
	add_stat_field(result, "fps", stats.frames());
	add_stat_field(result, "tris", stats.tris() / stats.frames());
	add_stat_field(result, "drawcalls:", stats.batches() / stats.frames());
	const Scene::Stats& scene_stats = engine_.scene().stats();
	add_stat_field(result, "aabbs:", scene_stats.aabbs());
	add_stat_field(result, "aabbs_visible:", scene_stats.aabbs_visible());
	add_stat_field(result, "nodes:", scene_stats.nodes());
	add_stat_field(result, "nodes_visible:", scene_stats.nodes_visible());
	add_stat_field(result, "parts:", scene_stats.parts());
	add_stat_field(result, "parts_visible:", scene_stats.parts_visible());

	return result;
}

void RDBGProcessor::process_get_data(std::string& result, const Data& data) const
{
	add_field(result, types_cast<std::string>(data.children.size()));
	for (size_t i = 0; i < data.children.size(); ++i)
	{
		add_field(result, data.children[i].name, data.children[i].type);
		add_field(result, "fields");
		process_get_data(result, data.children[i]);
	}
}

std::string RDBGProcessor::make_error(const char* message) const
{
	return std::string("err ") + message;
}

std::string RDBGProcessor::set_data(Data& data, size_t id, const std::vector<std::string>& subtypes, const std::string* args, size_t argc)
{
	const Data* field_data = find_data(data, subtypes);
	if (field_data == nullptr) return make_error("");
	bool result = false;
	switch (field_data->type)
	{
	case None:
		return make_error(error_invalid_argument);
	case Int:
		result = set(*field_data, id, convert<int>(args, argc));
		break;
	case Vector4:
		result = set(*field_data, id, convert<vec4>(args, argc));
		break;
	}
	if (result)
		return message_ok;
	return make_error("");
}

std::string RDBGProcessor::get_data(Data& data, size_t id, const std::vector<std::string>& subtypes)
{
    NOT_IMPLEMENTED(data);
    NOT_IMPLEMENTED(id);
    NOT_IMPLEMENTED(subtypes);
	return make_error("");
}

const RDBGProcessor::Data* RDBGProcessor::find_data(const Data& root, const std::vector<std::string>& names) const
{
	if (names.empty()) return nullptr;
	if (names.size() == 1) return &root;
	bool process = true;
	const Data* parent = &root;
	size_t index = 1;
	while (process)
	{
		size_t i = 0;
		for (; i < parent->children.size(); ++i)
		{
			if (parent->children[i].name == names[index])
			{
				parent = &(parent->children[i]);
				if (++index == names.size())
					return parent;
				break;
			}
		}
		if (i == parent->children.size())
			process = false;
	}
	return nullptr;
}

void RDBGProcessor::update_profiler_data()
{
	std::string result;
	result.reserve(1024);
	const std::vector<Profiler::Data>& data = MainProfiler::instance().data();
	const std::string separator = " ";
	for (size_t i = 0, size = data.size(); i < size; ++i)
	{
		result += std::string(data[i].name.c_str());
		result += separator;
		result += types_cast<std::string>(data[i].id);
		result += separator;
		result += types_cast<std::string>(data[i].parent_id);
		result += separator;
		result += types_cast<std::string>(data[i].count);
		result += separator;
		result += types_cast<std::string>(data[i].interval);
		result += separator;
	}
	profiler_data_ = result;
}

bool RDBGThread::start_impl()
{
	return server_socket_.open(detail::rdbg_server_port);
}

void RDBGThread::process_impl()
{
	if (!server_socket_.connect())
		return;
	INFO_LOG("Client connected");
	while (true)
	{
		char length_buffer[4];
		if (!server_socket_.read(length_buffer, 4))
			return;
		uint32_t length = construct(length_buffer);
		char buffer[detail::max_buffer_length];
		::memset(buffer, 0, sizeof(buffer));
		if (!server_socket_.read(buffer, min(length, sizeof(buffer))))
			return;
		const std::string& answer = processor_.process_command(buffer);
		split(answer.size(), length_buffer);
		if (!server_socket_.write(length_buffer, 4))
			return;
		server_socket_.write(answer.c_str(), answer.size());
	}
}

bool RDBGEngine::start()
{
	return thread_.start();
}

std::vector<GlobalVars::Data> GlobalVars::data() const
{
	std::vector<Data> result;
	result.reserve(vars_.size());
	for (VarsMap::const_iterator it = vars_.begin(); it != vars_.end(); ++it)
		result.push_back(it->second);
	return result;
}

void GlobalVars::set(const char* name, const std::string* args, size_t args_number)
{
    NOT_IMPLEMENTED(args_number);
	VarsMap::iterator it = vars_.find(hash(name));
	if (it == vars_.end()) return;
	Data& data = it->second;
	switch (data.type)
	{
	case Int: data.value = args[0]; break;
	case None: return;
	default: return;
	}
	data.changed = true;
}

}
