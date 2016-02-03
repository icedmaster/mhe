#include "res/shader_program_loader.hpp"

#include "render/context.hpp"
#include "utils/strutils.hpp"
#include "utils/file_utils.hpp"

namespace mhe {

namespace detail {

const char* vertex_shader_tag = "[vertex]";
const char* fragment_shader_tag = "[fragment]";
const char* compute_shader_tag = "[compute]";
const char* include_tag = "[include";
const char* defs_tag = "[defs";
const char* sampler_tag = "[sampler";
const char* image_tag = "[image";
const char* uniform_tag = "[uniform";
const char* var_tag = "[var";
const char* version_tag = "[version";
const char* shader_extension = ".glsl";
const char* texture_buffer_sampler = "samplerBuffer";

const char* tags[] = {vertex_shader_tag, fragment_shader_tag, compute_shader_tag};

struct LoadContext
{
    std::string filename;
    std::string version;
    size_t level;
    bool tag_found;
};

std::string load_shader_impl(ShaderInitializationParams& params, const std::vector<std::string>& data, const std::string& tag, LoadContext& load_context);

const char* get_shader_header(const Context* context)
{
    uint major = context->driver.major_version_need();
    uint minor = context->driver.minor_version_need();
    if (major == 4 && minor == 5)
        return "#version 450 core";
    else if (major == 4 && minor == 3)
        return "#version 430 core";
    return "#version 330 core";
}

const char* get_basic_version_header()
{
    return "#version 330 core";
}

std::string get_version_header(const std::string& version)
{
    std::string header = "#version " + version + " core";
    return header;
}

std::string parse_include(ShaderInitializationParams& params, const std::string& tag, const std::string& shadertag, LoadContext& load_context)
{
    size_t first = tag.find_first_of('"');
    size_t last = tag.find_last_of('"');
    if (first == std::string::npos || last == std::string::npos)
        return std::string();
    const std::string& filename = tag.substr(first + 1, last - first - 1);

    std::string fullname = utils::path_join(utils::get_file_path(load_context.filename), filename);
    std::ifstream f(fullname.c_str());
    if (!f.is_open()) return std::string();
    const std::vector<std::string>& content = utils::read_lines(f);
    f.close();
    return load_shader_impl(params, content, shadertag, load_context);
}

std::string parse_sampler(ShaderInitializationParams& params, const std::string& tag)
{
    size_t first = tag.find('[');
    ASSERT(first != std::string::npos, "Invalid shader source:" << tag);
    size_t last = tag.find(']');
    ASSERT(last != std::string::npos, "Invalid shader source:" << tag);
    const std::vector<std::string>& data = utils::split(tag.substr(first + 1, last - first - 1), " ");
    // 3 == sampler type, name, index, for example [sampler2D albedo 0]
    ASSERT(data.size() == 3 && data[0].find("sampler") != std::string::npos, "Invalid shader source:" << tag);
    SamplerData sampler_data;
    sampler_data.name = data[1];
    sampler_data.index = types_cast<size_t>(data[2]);
    if (data[0] == texture_buffer_sampler)
        params.texture_buffers.push_back(sampler_data);
    else
        params.samplers.push_back(sampler_data);
    return "uniform " + data[0] + " " + data[1] + ";\n";
}

std::string parse_image(ShaderInitializationParams& params, const std::string& tag)
{
    size_t first = tag.find('[');
    ASSERT(first != std::string::npos, "Invalid shader source:" << tag);
    size_t last = tag.find(']');
    ASSERT(last != std::string::npos, "Invalid shader source:" << tag);
    const std::vector<std::string>& data = utils::split(tag.substr(first + 1, last - first - 1), " ");
    // 3 == image type, name, index, for example [image2D src 0]
    ASSERT(data.size() >= 3 && data[0].find("image") != std::string::npos, "Invalid shader source:" << tag);
    ImageData image_data;
    image_data.name = data[1];
    image_data.index = types_cast<size_t>(data[2]);
    params.images.push_back(image_data);

    std::string result = "uniform " + data[0] + " " + data[1];
    std::string qualifiers;
    for (size_t i = 3, size = data.size(); i < size; ++i)
    {
        qualifiers += data[i];
        qualifiers += " ";
    }
    result = qualifiers + result + ";\n";
    return result;
}

std::string parse_uniform(ShaderInitializationParams& params, const std::string& tag)
{
    size_t first = tag.find('[');
    ASSERT(first != std::string::npos, "Invalid shader source:" << tag);
    size_t last = tag.find(']');
    ASSERT(last != std::string::npos, "Invalid shader source:" << tag);
    const std::vector<std::string>& data = utils::split(tag.substr(first + 1, last - first - 1), " ");
    // 4 == uniform, name, index, update type [uniform global_transform 0 perframe]
    ASSERT(data.size() == 4 && data[0] == "uniform", "Invalid shader source:" << tag);
    UniformData uniform_data;
    uniform_data.name = data[1];
    uniform_data.index = types_cast<size_t>(data[2]);
    NOT_IMPLEMENTED(data[3]);
    params.uniforms.push_back(uniform_data);
    return "";
}

std::string parse_variable(ShaderInitializationParams& params, const std::string& tag)
{
    size_t first = tag.find('[');
    ASSERT(first != std::string::npos, "Invalid shader source:" << tag);
    size_t last = tag.find(']');
    ASSERT(last != std::string::npos, "Invalid shader source:" << tag);
    const std::vector<std::string>& data = utils::split(tag.substr(first + 1, last - first - 1), " ");
    ASSERT(data.size() == 3 && data[0] == "var", "Invalid shader source:" << tag);
    params.variables.add(string(data[1].c_str()), string(data[2].c_str()));
    return "#define " + data[1] + " " + data[2];
}

std::string parse_version(ShaderInitializationParams& params, const std::string& tag)
{
    size_t first = tag.find('[');
    ASSERT(first != std::string::npos, "Invalid shader source:" << tag);
    size_t last = tag.find(']');
    ASSERT(last != std::string::npos, "Invalid shader source:" << tag);
    const std::vector<std::string>& data = utils::split(tag.substr(first + 1, last - first - 1), " ");
    ASSERT(data.size() == 2 && data[0] == "version", "Invalid shader source:" << tag);
    params.version = data[1];
    return "";
}

void update_shader_data(ShaderInitializationParams& params, const Context* context)
{
    if (!params.csdata.empty())
        params.csdata = get_shader_header(context) + std::string("\n") + params.csdata;
    else
    {
        const std::string& version = params.version.empty() ? get_basic_version_header() : get_version_header(params.version);
        const std::string& header = version + std::string("\n");
        params.vsdata = header + params.vsdata;
        params.fsdata = header + params.fsdata;
    }
}

std::string load_shader_impl(ShaderInitializationParams& params, const std::vector<std::string>& data, const std::string& tag, LoadContext& load_context)
{
    std::string actual_data;

    enum
    {
        state_not_found,
        state_found,
        state_incorrect_found
    };

    int state = state_not_found;

    for (size_t i = 0; i < data.size(); ++i)
    {
        const std::string& s = utils::trim_copy(data[i]);

        if (s.find(include_tag) != std::string::npos)
        {
            ++load_context.level;
            actual_data += parse_include(params, s, tag, load_context);
            --load_context.level;
            continue;
        }

        if (s.find(defs_tag) != std::string::npos)
            continue;

        if (s.find(sampler_tag) != std::string::npos)
        {
            actual_data += parse_sampler(params, s);
            continue;
        }

        if (s.find(uniform_tag) != std::string::npos)
        {
            actual_data += parse_uniform(params, s);
            continue;
        }

        if (s.find(image_tag) != std::string::npos)
        {
            actual_data += parse_image(params, s);
            continue;
        }

        if (s.find(var_tag) != std::string::npos)
        {
            actual_data += parse_variable(params, s);
            continue;
        }

        if (s.find(version_tag) != std::string::npos)
        {
            actual_data += parse_version(params, s);
            continue;
        }

        if (s == tag)
            state = state_found;
        else
        {
            for (size_t j = 0; j < 2; ++j)
            {
                if (s == tags[j])
                {
                    if (state == state_found)
                        return actual_data;
                    else state = state_incorrect_found;
                    break;
                }
            }

            if (state != state_incorrect_found)
                actual_data += (s + "\n");
        }
    }
    if (state == state_found)
        load_context.tag_found = true;
    if (!load_context.tag_found && load_context.level == 0)
        actual_data.clear();
    return actual_data;
}

void parse_defs(std::vector<UberShader::Info>& infos, const std::vector<std::string>& data)
{
    size_t offset = 1;
    for (size_t i = 0; i < data.size(); ++i)
    {
        if (data[i].find(defs_tag) == std::string::npos) continue;
        // definition looks like [defs name low_index high_index]
        size_t first = data[i].find("[");
        size_t last = data[i].find("]");
        if (first == std::string::npos || last == std::string::npos)
        {
            ASSERT(0, "Invalid definition:" << data[i]);
            continue;
        }
        const std::vector<std::string>& elements = utils::split(data[i].substr(first + 1, last - first - 1), " ");
        ASSERT(elements.size() == 4, "Invalid definition:" << data[i]);
        UberShader::Info info;
        info.name = elements[1];
        info.offset = offset;
        info.low = types_cast<size_t>(elements[2]);
        info.high = types_cast<size_t>(elements[3]);
        infos.push_back(info);
        offset *= (info.high - info.low + 1);
    }
}

} // namespace detail

bool ShaderProgramLoader::load(type& res, const std::string& name, const context_type* context)
{
    UberShader::IdType id = context->ubershader_pool.create();
    UberShader& ubershader = context->ubershader_pool.get(id);

    size_t defs_pos = name.find_first_of('&');
    const std::string& defs = defs_pos == std::string::npos ? std::string() : name.substr(defs_pos + 1, name.size() - defs_pos);
    const std::vector<std::string>& definitions = utils::split(defs, ",");

    const std::string& actual_filename = defs_pos == std::string::npos ? name  : name.substr(0, defs_pos);
    std::string filename_with_extension = actual_filename + detail::shader_extension;
    std::ifstream f(filename_with_extension.c_str());
    if (!f.is_open())
    {
        WARN_LOG("Can't open file:" << filename_with_extension);
        return false;
    }
    const std::vector<std::string>& file_content = utils::read_lines(f);
    f.close();

    std::vector<UberShader::Info> infos;
    detail::parse_defs(infos, file_content);
    size_t total = 1;
    if (!infos.empty())
    {
        const UberShader::Info& last = infos.back();
        total = last.offset * (last.high - last.low + 1);
    }

    bool result = false;
    for (size_t i = 0; i < total; ++i)
    {
        ShaderProgram::IdType shader_id = context->shader_pool.create();
        ShaderProgram& shader_program = context->shader_pool.get(shader_id);

        std::vector<std::string> data;
        data.reserve(1024);
        for (size_t j = 0; j < definitions.size(); ++j)
            data.push_back(std::string("#define ") + definitions[j]);

        std::string debug_defs;
        for (size_t j = 0; j < infos.size(); ++j)
        {
            size_t value = infos[j].low + (i / infos[j].offset) % (infos[j].high - infos[j].low + 1);
            std::string def = "#define " + infos[j].name + " " + types_cast<std::string>(value);
            debug_defs += def;
            debug_defs += " ";
            data.push_back(def);
        }

        data.insert(data.end(), file_content.begin(), file_content.end());

        ShaderInitializationParams params;
        detail::LoadContext load_context;
        load_context.filename = filename_with_extension;
        load_context.level = 0;
        load_context.tag_found = false;
        params.csdata = detail::load_shader_impl(params, data, detail::compute_shader_tag, load_context);
        if (params.csdata.empty())
        {
            params.vsdata = detail::load_shader_impl(params, data, detail::vertex_shader_tag, load_context);
            params.fsdata = detail::load_shader_impl(params, data, detail::fragment_shader_tag, load_context);
        }
        // TODO: parse GLSL errors to find the correct line with error
        
        const std::vector<std::string>& vs_tmp = utils::split(params.vsdata, "\n", true);
        const std::vector<std::string>& fs_tmp = utils::split(params.fsdata, "\n", true);
        const std::vector<std::string>& cs_tmp = utils::split(params.csdata, "\n", true);
        UNUSED(vs_tmp);
        UNUSED(fs_tmp);
        UNUSED(cs_tmp);

        detail::update_shader_data(params, context);

        result = shader_program.init(params);
        if (!result)
        {
            ASSERT(0, "Shader compilation failed: " << name << "\n" << debug_defs);
            continue;
        }

        ubershader.add(i, shader_id);
    }
    if (!result) return false;

    ubershader.set_infos(infos);
    res.shader_program_handle = ubershader.id();
    return true;
}

}
