#include "shader.hpp"

#include <fstream>
#include "utils/file_utils.hpp"
#include "utils/global_log.hpp"

namespace mhe {

bool Shader::load_file(const std::string& filename)
{
	std::ifstream f(filename.c_str());
	if (!f.is_open())
	{
		ERROR_LOG("Can't open shader file:" << filename);
		return false;
	}
	const std::string& content = utils::read_whole_file(f);
	f.close();
	return load_impl(content);
}

}
