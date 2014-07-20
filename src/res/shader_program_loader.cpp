#include "res/shader_program_loader.hpp"

#include "render/context.hpp"
#include "utils/strutils.hpp"
#include "utils/file_utils.hpp"

namespace mhe {

namespace detail {

const std::string vertex_shader_tag = "[vertex]";
const std::string fragment_shader_tag = "[fragment]";
const std::string shader_header = "#version 330 core";
const std::string shader_extension = ".glsl";

const std::string tags[] = {vertex_shader_tag, fragment_shader_tag};

std::string load_shader_impl(const std::vector<std::string>& data, const std::string& tag)
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
    return actual_data;
}

} // namespace detail

bool ShaderProgramLoader::load(type& res, const std::string& name, const context_type* context)
{
	ShaderProgram::IdType id = context->shader_pool.create();
	ShaderProgram& shader_program = context->shader_pool.get(id);

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

	std::vector<std::string> data;
	data.reserve(1024);
	data.push_back(detail::shader_header);
	for (size_t i = 0; i < definitions.size(); ++i)
		data.push_back(std::string("#define ") + definitions[i]);
	data.insert(data.end(), file_content.begin(), file_content.end());

	bool result = shader_program.init(detail::load_shader_impl(data, detail::vertex_shader_tag),
		detail::load_shader_impl(data, detail::fragment_shader_tag));
	if (result)
		res.shader_program_handle = id;
	return result;
}

}
