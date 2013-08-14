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
#ifndef MHE_USE_NATIVE_OPENGL
		(*glActiveTexture_)(texture);
#else
		::glActiveTexture(texture);
#endif
	}

	void glMultiTexCoord2f(GLenum target, GLfloat s, GLfloat t)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		(*glMultiTexCoord2f_)(target, s, t);
#else
        UNUSED(target); UNUSED(s); UNUSED(t);
#endif
	}

#ifdef MHE_OPENGL_HAS_SHADERS
	GLuint glCreateProgram()
	{
#ifndef MHE_USE_NATIVE_OPENGL
		return glCreateProgram_();
#else
		return ::glCreateProgram();
#endif
	}

	void glDeleteProgram(GLuint program)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glDeleteProgram_(program);
#else
		::glDeleteProgram(program);
#endif
	}

	GLuint glCreateShader(GLenum program)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		return glCreateShader_(program);
#else
		return ::glCreateShader(program);
#endif
	}
    
    void glDeleteShader(GLuint shader)
    {
#ifndef MHE_USE_NATIVE_OPENGL
		glDeleteShader_(shader);
#else
        ::glDeleteShader(shader);
#endif
    }

	void glShaderSource(GLuint shader,  GLsizei count,  const GLchar** string,  const GLint* length)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glShaderSource_(shader, count, string, length);
#else
		::glShaderSource(shader, count, string, length);
#endif
	}

	void glCompileShader(GLuint shader)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glCompileShader_(shader);
#else
		::glCompileShader(shader);
#endif
	}

	void glAttachShader(GLuint program, GLuint shader)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glAttachShader_(program, shader);
#else
		::glAttachShader(program, shader);
#endif
	}

	void glLinkProgram(GLuint program)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glLinkProgram_(program);
#else
		::glLinkProgram(program);
#endif
	}

	void glUseProgram(GLuint program)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glUseProgram_(program);
#else
		::glUseProgram(program);
#endif
	}

	void glValidateProgram(GLuint program)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glValidateProgram_(program);
#else
		::glValidateProgram(program);
#endif
	}

	void glGetShaderiv(GLuint shader, GLenum pname, GLint* params)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glGetShaderiv_(shader, pname, params);
#else
		::glGetShaderiv(shader, pname, params);
#endif
	}

	void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glGetShaderInfoLog_(shader, maxLength, length, infoLog);
#else
		::glGetShaderInfoLog(shader, maxLength, length, infoLog);
#endif
	}

	void glGetProgramiv(GLuint program, GLenum pname, GLint* params)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glGetProgramiv_(program, pname, params);
#else
		::glGetProgramiv(program, pname, params);
#endif
	}

	void glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glGetProgramInfoLog_(program, maxLength, length, infoLog);
#else
		::glGetProgramInfoLog(program, maxLength, length, infoLog);
#endif
	}

	GLint glGetUniformLocation(GLuint program, const GLchar* name)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		return glGetUniformLocation_(program, name);
#else
		return ::glGetUniformLocation(program, name);
#endif
	}

	void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glUniformMatrix4fv_(location, count, transpose, value);
#else
		::glUniformMatrix4fv(location, count, transpose, value);
#endif
	}

	void glUniform1i(GLint location, GLint value)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glUniform1i_(location, value);
#else
		::glUniform1i(location, value);
#endif
	}

	GLint glGetAttributeLocation(GLuint program, const GLchar* name)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		return glGetAttribLocation_(program, name);
#else
		return ::glGetAttribLocation(program, name);
#endif
	}

	void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized,
							   GLsizei stride, const GLvoid* pointer)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glVertexAttribPointer_(index, size, type, normalized, stride, pointer);
#else
		::glVertexAttribPointer(index, size, type, normalized, stride, pointer);
#endif
	}

	void glEnableVertexAttribArray(GLint attr)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glEnableVertexAttribArray_(attr);
#else
		::glEnableVertexAttribArray(attr);
#endif
	}
    
    void glDisableVertexAttribArray(GLint attr)
    {
#ifndef MHE_USE_NATIVE_OPENGL
		glDisableVertexAttribArray_(attr);
#else
        ::glDisableVertexAttribArray(attr);
#endif
    }

	void glGenBuffers(GLsizei n, GLuint* buffers)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glGenBuffers_(n, buffers);
#else
		::glGenBuffers(n, buffers);
#endif
	}

	void glDeleteBuffers(GLsizei n, GLuint* buffers)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glDeleteBuffers_(n, buffers);
#else
		::glDeleteBuffers(n, buffers);
#endif
	}

	void glBindBuffer(GLenum target, GLuint buffer)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glBindBuffer_(target, buffer);
#else
		::glBindBuffer(target, buffer);
#endif
	}

	void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glBufferData_(target, size, data, usage);
#else
		::glBufferData(target, size, data, usage);
#endif
	}

	void glGenVertexArrays(GLsizei n, GLuint* array)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glGenVertexArrays_(n, array);
#else
		#ifdef MHE_OPENGLES
		::glGenVertexArraysOES(n, array);
		#else
		#ifndef MHE_MACOS
		::glGenVertexArrays(n, array);
		#else
		::glGenVertexArraysAPPLE(n, array);
		#endif  // MHE_MACOS
		#endif  // MHE_OPENGLES
#endif
	}

	void glDeleteVertexArrays(GLsizei n, GLuint* array)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glDeleteVertexArrays_(n, array);
#else
		#ifdef MHE_OPENGLES
		::glDeleteVertexArraysOES(n, array);
		#else
		#ifndef MHE_MACOS
		::glDeleteVertexArrays(n, array);
		#else
		::glDeleteVertexArraysAPPLE(n, array);
		#endif // MHE_MACOS
		#endif // MHE_OPENGLES
#endif
	}

	void glBindVertexArray(GLuint array)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glBindVertexArray_(array);
#else
		#ifdef MHE_OPENGLES
		::glBindVertexArrayOES(array);
		#else
		#ifndef MHE_MACOS
		::glBindVertexArray(array);
		#else
		::glBindVertexArrayAPPLE(array);
		#endif // MHE_MACOS
		#endif // MHE_OPENGLES
#endif
	}
#endif // MHE_OPENGL_HAS_SHADERS
private:
	OpenGLExtensions() {}
	~OpenGLExtensions() {}

	template <class T>
	T load_extension(const char* name)
	{
#ifndef MHE_USE_NATIVE_OPENGL
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

#ifndef MHE_USE_NATIVE_OPENGL
	PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2f_;
    PFNGLACTIVETEXTUREARBPROC glActiveTexture_;
	PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTexture_;
	PFNGLCREATEPROGRAMPROC glCreateProgram_;
	PFNGLDELETEPROGRAMPROC glDeleteProgram_;
	PFNGLCREATESHADERPROC glCreateShader_;
	PFNGLDELETESHADERPROC glDeleteShader_;
	PFNGLSHADERSOURCEPROC glShaderSource_;
	PFNGLCOMPILESHADERPROC glCompileShader_;
	PFNGLATTACHSHADERPROC glAttachShader_;
	PFNGLLINKPROGRAMPROC glLinkProgram_;
	PFNGLUSEPROGRAMPROC glUseProgram_;
	PFNGLVALIDATEPROGRAMPROC glValidateProgram_;
	PFNGLGETSHADERIVPROC glGetShaderiv_;
	PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog_;
	PFNGLGETPROGRAMIVPROC glGetProgramiv_;
	PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog_;
	PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation_;
	PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv_;
	PFNGLUNIFORM1IPROC glUniform1i_;
	PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation_;
	PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer_;
	PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray_;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray_;
	PFNGLGENBUFFERSPROC glGenBuffers_;
	PFNGLDELETEBUFFERSPROC glDeleteBuffers_;
	PFNGLBINDBUFFERPROC glBindBuffer_;
	PFNGLBUFFERDATAPROC glBufferData_;
	PFNGLGENVERTEXARRAYSPROC glGenVertexArrays_;
	PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays_;
	PFNGLBINDVERTEXARRAYPROC glBindVertexArray_;
#endif
	std::map<std::string, bool> loaded_extensions_;
};

}}

#endif
