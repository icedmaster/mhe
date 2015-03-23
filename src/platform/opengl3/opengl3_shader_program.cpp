#include "platform/opengl3/opengl3_shader_program.hpp"

#include "utils/global_log.hpp"
#include "platform/opengl/opengl_extension.hpp"

namespace mhe {
namespace opengl {

void OpenGL3ShaderProgram::Shader::close()
{
	OpenGLExtensions::instance().glDeleteShader(id_);
}

bool OpenGL3ShaderProgram::Shader::init(GLenum type, const std::string& data)
{
	id_ = OpenGLExtensions::instance().glCreateShader(type);
	const GLchar* shader_data = data.c_str();
	GLint length = static_cast<GLint>(data.length());
	OpenGLExtensions::instance().glShaderSource(id_, 1, &shader_data, &length);
	OpenGLExtensions::instance().glCompileShader(id_);
	return check_status(GL_COMPILE_STATUS);
}

bool OpenGL3ShaderProgram::Shader::check_status(GLenum param) const
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

bool OpenGL3ShaderProgram::init(const std::string& vsdata, const std::string& fsdata, const ShaderInitializationParams& params)
{
	if (!vertex_shader_.init(GL_VERTEX_SHADER, vsdata))
		return false;
	if (!fragment_shader_.init(GL_FRAGMENT_SHADER, fsdata))
		return false;
	
	bool result = attach_shaders();
	if (result)
		init(params);
	return result;
}

void OpenGL3ShaderProgram::close()
{
	OpenGLExtensions::instance().glDeleteProgram(id_);
}

bool OpenGL3ShaderProgram::attach_shaders()
{
	id_ = OpenGLExtensions::instance().glCreateProgram();
	OpenGLExtensions::instance().glAttachShader(id_, vertex_shader_.id());
	OpenGLExtensions::instance().glAttachShader(id_, fragment_shader_.id());
	OpenGLExtensions::instance().glLinkProgram(id_);
	return check_status(GL_LINK_STATUS); 
}

bool OpenGL3ShaderProgram::check_status(GLenum param) const
{
	GLint status;
	OpenGLExtensions::instance().glGetProgramiv(id_, param, &status);
	if (status != GL_TRUE)
	{
		GLchar buffer[1024];
		GLint length = 0;
		OpenGLExtensions::instance().glGetProgramInfoLog(id_, sizeof(buffer), &length, buffer);
		ERROR_LOG("ShaderProgram " << id_ << " check status error:" << buffer);
		return false;
	}
	return true;
}

void OpenGL3ShaderProgram::set() const
{
	OpenGLExtensions::instance().glUseProgram(id_);
	for (size_t i = 0; i < 16; ++i)
	{
		if (texture_location_[i] == static_cast<GLuint>(-1)) continue;
		OpenGLExtensions::instance().glUniform1i(texture_location_[i], i);
	}
}

void OpenGL3ShaderProgram::init(const ShaderInitializationParams& params)
{
	memset(&texture_location_[0], 0xff, sizeof(texture_location_));

	// check samplers locations
	for (size_t i = 0; i < params.samplers.size(); ++i)
	{
		texture_location_[params.samplers[i].index] = OpenGLExtensions::instance().glGetUniformLocation(id_, params.samplers[i].name.c_str());
	}

	// check uniform locations
	for (size_t i = 0; i < params.uniforms.size(); ++i)
	{
		shader_bind_indexes_[params.uniforms[i].index] = OpenGLExtensions::instance().glGetUniformBlockIndex(id_, params.uniforms[i].name.c_str());
	}
}

GLuint OpenGL3ShaderProgram::uniform_location(size_t unit) const
{
	return shader_bind_indexes_[unit];
}

}}
