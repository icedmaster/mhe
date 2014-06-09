#ifndef __OPENGL3_SHADER_PROGRAM_HPP__
#define __OPENGL3_SHADER_PROGRAM_HPP__

#include "render/shader_program.hpp"

namespace mhe {
namespace opengl {

class OpenGL3ShaderProgram : public ShaderProgramImpl
{
public:
	bool init(const std::string& vsdata, const std::string& fsdata)
	{
		return false;
	}

	void close() {}

	GLuint id() const
	{
		return id_;
	}
private:
	GLuint id_;
};

}}

#endif
