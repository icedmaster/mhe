#include "debug/rdbg.hpp"

#include "core/types_cast.hpp"
#include "core/types.hpp"
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
const char* error_unknown_command = "Unknown command";
const char* error_not_implemented = "Not implemented";

enum Commands
{
    command_text,   // The actual command will be in the text message following next
    command_set,
    command_get,
    command_set_variable,
    command_get_variable,
    command_get_all_variables,
    command_get_profiler_data,
    command_get_stats
};

enum Results
{
    result_ok,
    result_error
};

const uint8_t cpu_profiler_data = 0;
const uint8_t gpu_profiler_data = 1;

const size_t cmd_length = 1;

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

template <class S>
string decode_string(ByteStream& stream)
{
    uint32_t len = construct(stream.read(4));
    string s(stream.read(len), len);
    return s;
}

template <class S>
inline void add_field(S& dst, const char* field)
{
    char n[4];
    split(strlen(field), n);
    dst.append(n, 4);
    dst.append(field, strlen(field));
}

template <class S>
inline void add_field(S& dst, const string& field)
{
    add_field(dst, field.c_str());
}

template <class S>
inline void add_field(S& dst, int n)
{
    char b[4];
    split(n, b);
    dst.append(b, 4);
}

template <class S>
inline void add_field(S& dst, size_t n)
{
    add_field<S>(dst, static_cast<int>(n));
}

template <class S>
inline void add_field(S& dst, uint32_t n)
{
    add_field<S>(dst, static_cast<int>(n));
}

template <class S>
inline void add_field(S& dst, float f)
{
    dst.append(reinterpret_cast<const char*>(&f), 4);
}

inline void add_stat_view(rdbgvector& dst, const Stats::View& view)
{
    add_field(dst, view.name);
    add_field(dst, static_cast<int>(view.fields.size()));
    for (size_t i = 0, size = view.fields.size(); i < size; ++i)
    {
        add_field(dst, view.fields[i].name);
        add_field(dst, view.fields[i].type);

        char value[80];
        size_t value_len = serialize(value, view.fields[i].type, view.fields[i].value);
        add_field(dst, static_cast<int>(value_len));
        dst.append(value, value_len);
    }
}
}

bool RDBGProcessor::init()
{
    global_vars_copy_ = GlobalVars::instance().data();
    for (size_t i = 0, size = global_vars_copy_.size(); i < size; ++i)
    {
        add_field(all_variables_, global_vars_copy_[i].name);
        add_field(all_variables_, global_vars_copy_[i].type);
    }

    return true;
}

void RDBGProcessor::update()
{
    lock_guard lock(mutex_);

    update_variables_data();
    update_profiler_data();
    update_gpu_profiler_data();
    update_stats_data();
}

RDBGData RDBGProcessor::process_command(const RDBGData& cmd)
{
    lock_guard lock(mutex_);
    
    RDBGData result;
    if (process_default_command(result, cmd))
        return result;

    if (process_var(result, cmd))
        return result;

    return make_error(error_not_implemented);
}

bool RDBGProcessor::process_default_command(RDBGData& result, const RDBGData& data)
{
    if (data.cmd == command_get_all_variables)
        result = process_get_all_command(data);
    else if (data.cmd == command_get_profiler_data)
        result = process_profiler_result_command(data);
    else if (data.cmd == command_get_stats)
        result = process_stats_command(data);
    else return false;
    return true;
}

bool RDBGProcessor::process_var(RDBGData& result, const RDBGData& cmd)
{
    if (cmd.cmd == command_set_variable)
        return set_var(result, cmd);
    else if (cmd.cmd == command_get_variable)
        return get_var(result, cmd);
    return false;
}

bool RDBGProcessor::set_var(RDBGData& result, const RDBGData& cmd)
{
    ByteStream stream(cmd.data.data(), cmd.data.size());
    const string& varname = decode_string<string>(stream);
    GlobalVars::instance().set(varname, stream);
    result.cmd = result_ok;
    return true;
}

bool RDBGProcessor::get_var(RDBGData& result, const RDBGData& /*cmd*/)
{
    result = make_error(error_unknown_command);
    return true;
}

RDBGData RDBGProcessor::process_get_all_command(const RDBGData&)
{
    RDBGData result;
    result.cmd = result_ok;
    result.data = all_variables_;
    return result;
}

RDBGData RDBGProcessor::process_profiler_result_command(const RDBGData& data)
{
    ASSERT(data.data.size() == 1, "Invalid request for profilers data");
    RDBGData result;
    result.cmd = result_ok;
    const rdbgvector& res = data.data[0] == cpu_profiler_data ? profiler_data_ : gpu_profiler_data_;
    result.data.insert(result.data.end(), res.begin(), res.end());
    return result;
}

RDBGData RDBGProcessor::process_stats_command(const RDBGData&)
{
    RDBGData res;
    res.cmd = result_ok;
    res.data = stats_data_;
    return res;
}

/*void RDBGProcessor::process_get_data(std::string& result, const Data& data) const
{
    add_field(result, types_cast<std::string>(data.children.size()));
    for (size_t i = 0; i < data.children.size(); ++i)
    {
        add_field(result, data.children[i].name);
    }
}*/

RDBGData RDBGProcessor::make_error(const char* message) const
{
    RDBGData res;
    res.cmd = result_error;
    res.data.insert(res.data.end(), message, message + strlen(message));
    return res;
}

/*RDBGData RDBGProcessor::set_data(RDBGProcessor::Data& data, size_t id, const std::vector<std::string>& subtypes, const std::string* args, size_t argc)
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
    return make_error(error_not_implemented);
}*/

/*RDBGData RDBGProcessor::get_data(Data& data, size_t id, const std::vector<std::string>& subtypes)
{
    NOT_IMPLEMENTED(data);
    NOT_IMPLEMENTED(id);
    NOT_IMPLEMENTED(subtypes);
    return make_error("");
}*/

/*const RDBGProcessor::Data* RDBGProcessor::find_data(const Data& root, const std::vector<std::string>& names) const
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
}*/

void RDBGProcessor::update_profiler_data()
{
    profiler_data_.clear();
    profiler_data_.push_back(cpu_profiler_data);
    const std::vector<Profiler::Data>& data = MainProfiler::instance().data();
    add_field(profiler_data_, data.size());
    for (size_t i = 0, size = data.size(); i < size; ++i)
    {
        add_field(profiler_data_, data[i].id);
        add_field(profiler_data_, data[i].parent_id);
        add_field(profiler_data_, data[i].name);
        add_field(profiler_data_, data[i].count);
        add_field(profiler_data_, data[i].interval);
    }
}

void RDBGProcessor::update_gpu_profiler_data()
{
    const GPUProfiler::Nodes& nodes = MainGPUProfiler::instance().nodes();
    gpu_profiler_data_.clear();
    gpu_profiler_data_.push_back(gpu_profiler_data);
    add_field(gpu_profiler_data_, nodes.size());
    for (size_t i = 0, size = nodes.size(); i < size; ++i)
    {
        const GPUProfiler::Node& node = nodes[i];
        add_field(gpu_profiler_data_, node.node->id());
        add_field(gpu_profiler_data_, node.parent_id);
        add_field(gpu_profiler_data_, node.name);
        add_field(gpu_profiler_data_, 1);
        add_field(gpu_profiler_data_, node.interval);
    }
}

void RDBGProcessor::update_variables_data()
{
    const vector<GlobalVars::Data>& vars = GlobalVars::instance().data();
    ASSERT(vars.size() == global_vars_copy_.size(), "Global variables have been changed");
    for (size_t i = 0, size = vars.size(); i < size; ++i)
    {
        if (global_vars_copy_[i].changed)
        {
            GlobalVars::instance().update_data(i, global_vars_copy_[i].value);
            global_vars_copy_[i].changed = false;
        }
        else
        {
            global_vars_copy_[i].value = vars[i].value;
        }
    }
}

void RDBGProcessor::update_stats_data()
{
    stats_data_.clear();
    Stats& stats = engine_.stats();
    size_t views_number = stats.views_number();
    add_field(stats_data_, views_number);
    for (size_t i = 0; i < views_number; ++i)
        add_stat_view(stats_data_, stats.view(i));
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
    while (!finished())
    {
        char length_buffer[4];
        if (!server_socket_.read(length_buffer, 4))
            return;
        uint32_t length = construct(length_buffer);
        ASSERT(length >= cmd_length, "Invalid input data length");

        RDBGData data;
        data.data.clear();
        size_t readlen = mhe::min(length - cmd_length, data.data.capacity());
        data.data.resize(readlen);
        if (!server_socket_.read(reinterpret_cast<char*>(&data.cmd), cmd_length))
            return;
        if (readlen != 0 && !server_socket_.read(&data.data[0], readlen))
            return;
        const RDBGData& answer = processor_.process_command(data);
        split(answer.data.size() + cmd_length, length_buffer);
        if (!server_socket_.write(length_buffer, 4))
            return;
        if (!server_socket_.write(reinterpret_cast<const char*>(&answer.cmd), cmd_length))
            return;
        server_socket_.write(answer.data.data(), answer.data.size());
    }
    INFO_LOG("Client disconnected");
}

bool RDBGEngine::start()
{
    if (!processor_.init())
        return false;
    return thread_.start();
}

void RDBGEngine::stop()
{
    thread_.stop();
}

void RDBGEngine::update()
{
    processor_.update();
}

const vector<GlobalVars::Data>& GlobalVars::data() const
{
    return vars_;
}

void GlobalVars::set(const string& name, ByteStream& stream)
{
    NameToVectorMap::iterator it = names_indices_.find(name);
    if (it == names_indices_.end()) return;
    Data& data = vars_[it->value];
    switch (data.type)
    {
    case Int: data.value = types_cast<string>(construct(stream.read(4))); break;
    case None: return;
    default: NOT_IMPLEMENTED(data); return;
    }
    data.changed = true;
}

}
