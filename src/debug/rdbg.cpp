#include "debug/rdbg.hpp"

#include "core/types_cast.hpp"
#include "utils/strutils.hpp"

namespace mhe {

namespace {
const char* error_invalid_command = "Invalid command";
const char* error_unknown_command = "Unknown command";
const char* error_invalid_argument = "Invalid argument";

const char* message_ok = "ok";

const char* set_command = "s";
const char* get_command = "g";

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
}

std::string RDBGProcessor::process_command(const std::string& cmd)
{
	const std::vector<std::string>& args = utils::split(cmd, " ");
	if (args.empty())
		return make_error(error_invalid_command);
	// TODO: parse default commands
	const std::string& type_name = args[0];
	DataMap::iterator it = data_.find(hash(type_name));
	if (it == data_.end())
		return make_error(error_unknown_command);
	size_t id = types_cast<size_t>(args[1]);
	// The third argument should be whether we get or set data
	if (args[2] == set_command)
		return set_data(it->second, id, &args[3], args.size() - 3);
	else if (args[2] == get_command)
		return get_data(it->second, id);
	return make_error(error_invalid_argument);
}

std::string RDBGProcessor::make_error(const char* message) const
{
	return std::string("err ") + message;
}

std::string RDBGProcessor::set_data(Data& data, size_t id, const std::string* args, size_t argc)
{
	bool result = false;
	switch (data.type)
	{
	case None:
		return make_error(error_invalid_argument);
	case Int:
		result = set(data, id, convert<int>(args, argc));
		break;
	case Vector4:
		result = set(data, id, convert<vec4>(args, argc));
		break;
	}
	if (result)
		return message_ok;
	return make_error("");
}

std::string RDBGProcessor::get_data(Data& data, size_t id)
{
	return make_error("");
}

void RDBGThread::process_impl()
{

}

}
