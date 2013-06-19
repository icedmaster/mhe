#include "platform/opengl3/opengl3_shader_program.hpp"

#include "platform/opengl/opengl_extension.hpp"
#include "utils/global_log.hpp"

namespace mhe {
namespace opengl {

OpenGL3ShaderProgram::OpenGL3ShaderProgram() :
	id_(0), vertex_(Shader::vertex_shader), fragment_(Shader::fragment_shader)
{}

OpenGL3ShaderProgram::~OpenGL3ShaderProgram()
{
	remove_program();
}

bool OpenGL3ShaderProgram::load_files(const std::string& vertex_filename,
									   const std::string& fragment_filename)
{
	// init shaders
	if (!vertex_.load_file(vertex_filename))
	{
		ERROR_LOG("Create vertex shader with file:" << vertex_filename << " error");
		return false;
	}
	if (!fragment_.load_file(fragment_filename))
	{
		ERROR_LOG("Create fragment shader with file:" << fragment_filename << " error");
		return false;
	}
	return attach_shaders();
}

bool OpenGL3ShaderProgram::set_shaders(const Shader& vertex_shader, const Shader& fragment_shader)
{
	vertex_ = static_cast<const OpenGL3Shader&>(vertex_shader);
	fragment_ = static_cast<const OpenGL3Shader&>(fragment_shader);
	return attach_shaders();
}

bool OpenGL3ShaderProgram::attach_shaders()
{
    // first, create program
	if (!id_)
	{
		id_ = OpenGLExtensions::instance().glCreateProgram();
		if (!id_)
		{
			WARN_LOG("Create shader program error");
			return false;
		}
	}
	OpenGLExtensions::instance().glAttachShader(id_, vertex_.id_);
	OpenGLExtensions::instance().glAttachShader(id_, fragment_.id_);
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

void OpenGL3ShaderProgram::remove_program()
{
	OpenGLExtensions::instance().glDeleteProgram(id_);
}

bool OpenGL3ShaderProgram::set_uniform(const std::string& name, const matrixf& value)
{
    GLint uniform_position = OpenGLExtensions::instance().glGetUniformLocation(id_, name.c_str());
    if (uniform_position >= 0)
    {
        OpenGLExtensions::instance().glUniformMatrix4fv(uniform_position, 1, GL_FALSE, value.get());
        return true;
    }
    return false;
}

bool OpenGL3ShaderProgram::set_uniform(const std::string& name, int value)
{
    GLint uniform_position = OpenGLExtensions::instance().glGetUniformLocation(id_, name.c_str());
    if (uniform_position >= 0)
    {
        OpenGLExtensions::instance().glUniform1i(uniform_position, value);
        return true;
    }
    return false;
}

bool OpenGL3ShaderProgram::set_attribute(const std::string& name, const v3d& value)
{
	return set_attribute(name, value.get(), 3);
}

bool OpenGL3ShaderProgram::set_attribute(const std::string& name, const vector4<float>& value)
{
	return set_attribute(name, value.get(), 4);
}

bool OpenGL3ShaderProgram::set_attribute(const std::string& name, const float* data, cmn::uint size)
{
	return set_attribute(name, data, size, 0, 0);
}

bool OpenGL3ShaderProgram::set_attribute(const std::string& name, const float* /*data*/, cmn::uint size,
										  size_t offset, size_t stride)
{
    GLint attr_location = OpenGLExtensions::instance().glGetAttributeLocation(id_, name.c_str());
    if (attr_location >= 0)
    {
        OpenGLExtensions::instance().glVertexAttribPointer(attr_location, size, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(offset));
		OpenGLExtensions::instance().glEnableVertexAttribArray(attr_location);
        return true;
    }
    return false;
}
    
bool OpenGL3ShaderProgram::disable_attribute(const std::string& name)
{
    GLint attr_location = OpenGLExtensions::instance().glGetAttributeLocation(id_, name.c_str());
    if (attr_location >= 0)
    {        
		OpenGLExtensions::instance().glDisableVertexAttribArray(attr_location);
        return true;
    }
    return false;
}

}}
