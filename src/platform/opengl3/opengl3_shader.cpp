#include "platform/opengl3/opengl3_shader.hpp"

#include <fstream>
#include "platform/opengl/opengl_extension.hpp"
#include "utils/file_utils.hpp"
#include "utils/global_log.hpp"

namespace mhe {
namespace opengl {

OpenGL3Shader::OpenGL3Shader(ShaderType type) :
	Shader(type), id_(0)
{}

OpenGL3Shader::~OpenGL3Shader()
{
	remove_shader();
}

bool OpenGL3Shader::load_impl(const std::string& data)
{
	if (!id_)
	{
		id_ = OpenGLExtensions::instance().glCreateShader(opengl_shader_type());
		if (!id_)
		{
			WARN_LOG("Can't create shader");
			return false;
		}
	}
    const char* shader_data = data.c_str();
    GLint length = static_cast<GLint>(data.length());
	OpenGLExtensions::instance().glShaderSource(id_, 1, &shader_data, &length);
	return compile();
}

bool OpenGL3Shader::compile()
{
	OpenGLExtensions::instance().glCompileShader(id_);
	return check_status(GL_COMPILE_STATUS);		
}

void OpenGL3Shader::remove_shader()
{
	OpenGLExtensions::instance().glDeleteShader(id_);
}

bool OpenGL3Shader::check_status(GLenum param) const
{
	GLint status;
	OpenGLExtensions::instance().glGetShaderiv(id_, param, &status);
	if (status != GL_TRUE)
	{
		GLchar buffer[1024];
		GLint length = 0;
		OpenGLExtensions::instance().glGetShaderInfoLog(id_, sizeof(buffer), &length, buffer);
		ERROR_LOG("Shader " << id_ << " check status error:" << buffer);
		return false;
	}
	return true;
}

GLuint OpenGL3Shader::opengl_shader_type() const
{
	switch (type())
	{
		case fragment_shader:
			return GL_FRAGMENT_SHADER;
		default:
			return GL_VERTEX_SHADER;
	}
}

}}
