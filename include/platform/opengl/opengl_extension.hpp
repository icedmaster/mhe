#ifndef __OPENGL_EXTENSION_HPP__
#define __OPENGL_EXTENSION_HPP__

#include <string>
#include <map>
#include "mhe_gl.hpp"
#include "platform/platform_so.hpp"
#include "utils/singleton.hpp"
#include "utils/unused.hpp"

namespace mhe {
namespace opengl {

class OpenGLExtensions : public utils::Singleton<OpenGLExtensions>
{
	friend class utils::Singleton<OpenGLExtensions>;
public:
	void init_extensions();
	bool is_extension_supported(const std::string& ext_name) const;
	std::string get_supported_extensions() const;

	//
	void glActiveTexture(GLenum texture)
	{
#ifndef MHE_OPENGLES
		(*glActiveTexture_)(texture);
#else
		::glActiveTexture(texture);
#endif
	}

	void glMultiTexCoord2f(GLenum target, GLfloat s, GLfloat t)
	{
#ifndef MHE_OPENGLES
		(*glMultiTexCoord2f_)(target, s, t);
#else
        UNUSED(target); UNUSED(s); UNUSED(t);
#endif
	}

	GLuint glCreateProgram()
	{
#ifdef MHE_OPENGLES
		return ::glCreateProgram();
#endif
	}

	void glDeleteProgram(GLuint program)
	{
#ifdef MHE_OPENGLES
		::glDeleteProgram(program);
#endif
	}

	GLuint glCreateShader(GLenum program)
	{
#ifdef MHE_OPENGLES
		return ::glCreateShader(program);
#endif
	}
    
    void glDeleteShader(GLuint shader)
    {
#ifdef MHE_OPENGLES
        ::glDeleteShader(shader);
#endif
    }

	void glShaderSource(GLuint shader,  GLsizei count,  const GLchar** string,  const GLint* length)
	{
#ifdef MHE_OPENGLES
		::glShaderSource(shader, count, string, length);
#endif
	}

	void glCompileShader(GLuint shader)
	{
#ifdef MHE_OPENGLES
		::glCompileShader(shader);
#endif
	}

	void glAttachShader(GLuint program, GLuint shader)
	{
#ifdef MHE_OPENGLES
		::glAttachShader(program, shader);
#endif
	}

	void glLinkProgram(GLuint program)
	{
#ifdef MHE_OPENGLES
		::glLinkProgram(program);
#endif
	}

	void glUseProgram(GLuint program)
	{
#ifdef MHE_OPENGLES
		::glUseProgram(program);
#endif
	}

	void glValidateProgram(GLuint program)
	{
#ifdef MHE_OPENGLES
		::glValidateProgram(program);
#endif
	}

	void glGetShaderiv(GLuint shader, GLenum pname, GLint* params)
	{
#ifdef MHE_OPENGLES
		::glGetShaderiv(shader, pname, params);
#endif
	}

	void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
	{
#ifdef MHE_OPENGLES
		::glGetShaderInfoLog(shader, maxLength, length, infoLog);
#endif
	}

	void glGetProgramiv(GLuint program, GLenum pname, GLint* params)
	{
#ifdef MHE_OPENGLES
		::glGetProgramiv(program, pname, params);
#endif
	}

	void glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
	{
#ifdef MHE_OPENGLES
		::glGetProgramInfoLog(program, maxLength, length, infoLog);
#endif
	}

	GLint glGetUniformLocation(GLuint program, const GLchar* name)
	{
#ifdef MHE_OPENGLES
		return ::glGetUniformLocation(program, name);
#endif
	}

	void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
	{
#ifdef MHE_OPENGLES
		::glUniformMatrix4fv(location, count, transpose, value);
#endif
	}

	void glUniform1i(GLint location, GLint value)
	{
#ifdef MHE_OPENGLES
		::glUniform1i(location, value);
#endif
	}

	GLint glGetAttributeLocation(GLuint program, const GLchar* name)
	{
#ifdef MHE_OPENGLES
		return ::glGetAttribLocation(program, name);
#endif
	}

	void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized,
							   GLsizei stride, const GLvoid* pointer)
	{
#ifdef MHE_OPENGLES
		::glVertexAttribPointer(index, size, type, normalized, stride, pointer);
#endif
	}

	void glEnableVertexAttribArray(GLint attr)
	{
#ifdef MHE_OPENGLES
		::glEnableVertexAttribArray(attr);
#endif
	}
    
    void glDisableVertexAttribArray(GLint attr)
    {
#ifdef MHE_OPENGLES
        ::glDisableVertexAttribArray(attr);
#endif
    }

	void glGenBuffers(GLsizei n, GLuint* buffers)
	{
#ifdef MHE_OPENGLES
		::glGenBuffers(n, buffers);
#endif
	}

	void glDeleteBuffers(GLsizei n, GLuint* buffers)
	{
#ifdef MHE_OPENGLES
		::glDeleteBuffers(n, buffers);
#endif
	}

	void glBindBuffer(GLenum target, GLuint buffer)
	{
#ifdef MHE_OPENGLES
		::glBindBuffer(target, buffer);
#endif
	}

	void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
	{
#ifdef MHE_OPENGLES
		::glBufferData(target, size, data, usage);
#endif
	}

	void glGenVertexArrays(GLsizei n, GLuint* array)
	{
#ifdef MHE_OPENGLES
		::glGenVertexArraysOES(n, array);
#endif
	}

	void glDeleteVertexArrays(GLsizei n, GLuint* array)
	{
#ifdef MHE_OPENGLES
		::glDeleteVertexArraysOES(n, array);
#endif
	}

	void glBindVertexArray(GLuint array)
	{
#ifdef MHE_OPENGLES
		::glBindVertexArrayOES(array);
#endif
	}
private:
	OpenGLExtensions() {}
	~OpenGLExtensions() {}

	template <class T>
	T load_extension(const char* name)
	{
#ifndef MHE_OPENGLES
		bool loaded = true;
		T res = reinterpret_cast<T>(impl::get_opengl_proc_addr(name));
		if (res == 0) loaded = false;
		loaded_extensions_[name] = loaded;
		return res;
#else
        UNUSED(name);
        return nullptr;
#endif
	}

	void get_str_extensions();

#ifndef MHE_OPENGLES
	PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2f_;
    PFNGLACTIVETEXTUREARBPROC glActiveTexture_;
	PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTexture_;
#endif
	std::map<std::string, bool> loaded_extensions_;
};

}}

#endif
