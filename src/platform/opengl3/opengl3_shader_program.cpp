#include "platform/opengl3/opengl3_shader_program.hpp"

#include "utils/global_log.hpp"
#include "platform/opengl/opengl_extension.hpp"
#include "platform/opengl/opengl_helpers.hpp"

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
    return check_status(type, GL_COMPILE_STATUS);
}

bool OpenGL3ShaderProgram::Shader::check_status(GLenum type, GLenum param) const
{
    GLint status;
    OpenGLExtensions::instance().glGetShaderiv(id_, param, &status);
    if (status != GL_TRUE)
    {
        GLchar buffer[1024];
        GLint length = 0;
        OpenGLExtensions::instance().glGetShaderInfoLog(id_, sizeof(buffer), &length, buffer);
        ERROR_LOG("Shader " << id_ << " type:" << enum_to_string(type) << " check status error:" << buffer);
        return false;
    }
    return true;
}

bool OpenGL3ShaderProgram::init(const ShaderInitializationParams& params)
{
    if (params.csdata.empty())
    {
        if (!vertex_shader_.init(GL_VERTEX_SHADER, params.vsdata))
            return false;
        if (!fragment_shader_.init(GL_FRAGMENT_SHADER, params.fsdata))
            return false;
        if (!params.gsdata.empty())
        {
            if (!geometry_shader_.init(GL_GEOMETRY_SHADER, params.gsdata))
                return false;
        }
    }
    else
    if (!compute_shader_.init(GL_COMPUTE_SHADER, params.csdata))
        return false;
    bool result = attach_shaders();
    if (result)
        init_params(params);
    return result;
}

void OpenGL3ShaderProgram::close()
{
    vertex_shader_.close();
    geometry_shader_.close();
    fragment_shader_.close();
    compute_shader_.close();
    OpenGLExtensions::instance().glDeleteProgram(id_);
}

bool OpenGL3ShaderProgram::attach_shaders()
{
    id_ = OpenGLExtensions::instance().glCreateProgram();
    if (!compute_shader_.id())
    {
        OpenGLExtensions::instance().glAttachShader(id_, vertex_shader_.id());
        OpenGLExtensions::instance().glAttachShader(id_, fragment_shader_.id());
        if (geometry_shader_.id())
            OpenGLExtensions::instance().glAttachShader(id_, geometry_shader_.id());
    }
    else
        OpenGLExtensions::instance().glAttachShader(id_, compute_shader_.id());
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
    CHECK_GL_ERRORS();
    for (size_t i = 0; i < 16; ++i)
    {
        if (texture_location_[i] == static_cast<GLuint>(-1)) continue;
        OpenGLExtensions::instance().glUniform1i(texture_location_[i], i);
    }
}

void OpenGL3ShaderProgram::init_params(const ShaderInitializationParams& params)
{
    memset(&texture_location_[0], 0xff, sizeof(texture_location_));
    memset(&texture_buffer_location_[0], 0xff, sizeof(texture_buffer_location_));

    // check samplers locations
    for (size_t i = 0; i < params.samplers.size(); ++i)
    {
        texture_location_[params.samplers[i].index] = OpenGLExtensions::instance().glGetUniformLocation(id_, params.samplers[i].name.c_str());
    }

    // texture buffers
    for (size_t i = 0, size = params.texture_buffers.size(); i < size; ++i)
        texture_buffer_location_[params.texture_buffers[i].index] = OpenGLExtensions::instance().glGetUniformLocation(id_, params.texture_buffers[i].name.c_str());

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

GLuint OpenGL3ShaderProgram::texture_buffer_location(size_t unit) const
{
    return texture_buffer_location_[unit];
}

void OpenGL3ShaderProgram::print_uniforms()
{
    GLint number;
    OpenGLExtensions::instance().glGetProgramiv(id_, GL_ACTIVE_UNIFORMS, &number);
    for (GLint i = 0; i < number; ++i)
    {
        GLchar buffer[128];
        GLsizei length;
        GLint size;
        GLenum type;
        OpenGLExtensions::instance().glGetActiveUniform(id_, i, sizeof(buffer), &length, &size, &type, buffer);
        DEBUG_LOG("Shader id:" << id_ << " uniform:" << i << " name:" << buffer << " type:" << type << " size:" << size);
    }
}

}}
