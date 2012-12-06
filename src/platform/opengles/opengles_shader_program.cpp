#include "platform/opengles/opengles_shader_program.hpp"

#include "platform/opengl/opengl_extension.hpp"
#include "utils/global_log.hpp"

namespace mhe {
namespace opengl {

OpenGLESShaderProgram::OpenGLESShaderProgram() :
	id_(0), vertex_(Shader::vertex_shader), fragment_(Shader::fragment_shader)
{}

OpenGLESShaderProgram::~OpenGLESShaderProgram()
{
	remove_program();
}

bool OpenGLESShaderProgram::load_files(const std::string& vertex_filename,
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

bool OpenGLESShaderProgram::set_shaders(const Shader& vertex_shader, const Shader& fragment_shader)
{
	vertex_ = static_cast<const OpenGLESShader&>(vertex_shader);
	fragment_ = static_cast<const OpenGLESShader&>(fragment_shader);
	return attach_shaders();
}

bool OpenGLESShaderProgram::attach_shaders()
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

bool OpenGLESShaderProgram::check_status(GLenum param) const
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

void OpenGLESShaderProgram::remove_program()
{
	OpenGLExtensions::instance().glDeleteProgram(id_);
}

bool OpenGLESShaderProgram::set_uniform(const std::string& name, const matrixf& value)
{
    GLint uniform_position = OpenGLExtensions::instance().glGetUniformLocation(id_, name.c_str());
    if (uniform_position >= 0)
    {
        OpenGLExtensions::instance().glUniformMatrix4fv(uniform_position, 1, GL_FALSE, value.get());
        return true;
    }
    return false;
}

bool OpenGLESShaderProgram::set_attribute(const std::string& name, const v3d& value)
{
	return set_attribute(name, value.get(), 3);
}

bool OpenGLESShaderProgram::set_attribute(const std::string& name, const vector4<float>& value)
{
	return set_attribute(name, value.get(), 4);
}

bool OpenGLESShaderProgram::set_attribute(const std::string& name, const float* data, cmn::uint size)
{
    GLint attr_location = OpenGLExtensions::instance().glGetAttributeLocation(id_, name.c_str());
    if (attr_location >= 0)
    {
        OpenGLExtensions::instance().glVertexAttribPointer(attr_location, size, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const void**>(&data));
		OpenGLExtensions::instance().glEnableVertexAttribArray(attr_location);
        return true;
    }
    return false;
}
    
bool OpenGLESShaderProgram::disable_attribute(const std::string& name)
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
