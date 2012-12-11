#ifndef __SHADER_PROGRAM_HPP__
#define __SHADER_PROGRAM_HPP__

#include <string>
#include "shader.hpp"

namespace mhe {

class ShaderProgram
{
public:
	virtual ~ShaderProgram() {}

	virtual bool load_files(const std::string& vertex_filename,
							const std::string& fragment_filename) = 0;
	virtual bool set_shaders(const Shader& vertex_shader, const Shader& fragment_shader) = 0;
};

}

#endif
