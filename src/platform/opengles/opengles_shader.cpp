#include "platform/opengles/opengles_shader.hpp"

#include <fstream>
#include "platform/opengl/opengl_extension.hpp"
#include "utils/file_utils.hpp"
#include "utils/global_log.hpp"

namespace mhe {
namespace opengl {

OpenGLESShader::OpenGLESShader(ShaderType type) :
	Shader(type), id_(0)
{}

OpenGLESShader::~OpenGLESShader()
{
	remove_shader();
}

bool OpenGLESShader::load_impl(const std::string& data)
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

bool OpenGLESShader::compile()
{
	OpenGLExtensions::instance().glCompileShader(id_);
	return check_status(GL_COMPILE_STATUS);		
}

void OpenGLESShader::remove_shader()
{
	OpenGLExtensions::instance().glDeleteShader(id_);
}

bool OpenGLESShader::check_status(GLenum param) const
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

GLuint OpenGLESShader::opengl_shader_type() const
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
