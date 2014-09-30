#ifndef __OPENGL_EXTENSION_HPP__
#define __OPENGL_EXTENSION_HPP__

#include <string>
#include <map>
#include "mhe_gl.hpp"
#include "platform/platform_so.hpp"
#include "core/singleton.hpp"
#include "core/types.hpp"

namespace mhe {

class OpenGLExtensions : public Singleton<OpenGLExtensions>
{
	friend class Singleton<OpenGLExtensions>;
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

    void glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
    {
#ifndef MHE_USE_NATIVE_OPENGL
        glUniformMatrix3fv_(location, count, transpose, value);
#else
        ::glUniformMatrix3fv(location, count, transpose, value);
#endif
    }

    void glUniform3fv(GLint location, GLsizei count, const GLfloat *value)
    {
#ifndef MHE_USE_NATIVE_OPENGL
        glUniform3fv_(location, count, value);
#else
        ::glUniform3fv(location, count, value);
#endif
    }

    void glUniform4fv(GLint location, GLsizei count, const GLfloat *value)
    {
#ifndef MHE_USE_NATIVE_OPENGL
        glUniform4fv_(location, count, value);
#else
        ::glUniform4fv(location, count, value);
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

    void glUniform1f(GLint location, GLfloat value)
    {
#ifndef MHE_USE_NATIVE_OPENGL
        glUniform1f_(location, value);
#else
        ::glUniform1f(location, value);
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

	void glGetActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length,
		GLint *size, GLenum *type, GLchar *name)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glGetActiveUniform_(program, index, bufSize, length, size, type, name);
#else
        ::glGetActiveUniform(program, index, bufSize, length, size, type, name);
#endif
	}

	void glGenFramebuffers(GLsizei n, GLuint *ids)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glGenFramebuffers_(n, ids);
#else
        ::glGenFramebuffers(n, ids);
#endif
	}

	void glBindFramebuffer(GLenum target, GLuint framebuffer)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glBindFramebuffer_(target, framebuffer);
#else
        ::glBindFramebuffer(target, framebuffer);
#endif
	}

	void glFramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glFramebufferTexture_(target, attachment, texture, level);
#else
        ::glFramebufferTexture(target, attachment, texture, level);
#endif
	}

	GLuint glGetUniformBlockIndex(GLuint program, const GLchar *uniformBlockName)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		return glGetUniformBlockIndex_(program, uniformBlockName);
#else
        return ::glGetUniformBlockIndex(program, uniformBlockName);
#endif
	}

	void glGetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glGetActiveUniformBlockiv_(program, uniformBlockIndex, pname, params);
#else
        ::glGetActiveUniformBlockiv(program, uniformBlockIndex, pname, params);
#endif
	}

	void glGetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar **uniformNames, GLuint *uniformIndices)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glGetUniformIndices_(program, uniformCount, uniformNames, uniformIndices);
#else
        ::glGetUniformIndices(program, uniformCount, uniformNames, uniformIndices);
#endif
	}

	void glGetActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glGetActiveUniformsiv_(program, uniformCount, uniformIndices, pname, params);
#else
        ::glGetActiveUniformsiv(program, uniformCount, uniformIndices, pname, params);
#endif
	}

	void glBindBufferBase(GLenum target, GLuint index, GLuint buffer)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glBindBufferBase_(target, index, buffer);
#else
        ::glBindBufferBase(target, index, buffer);
#endif
	}

	void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glBufferSubData_(target, offset, size, data);
#else
		::glBufferSubData(target, offset, size, data);
#endif
	}

	void glUniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glUniformBlockBinding_(program, uniformBlockIndex, uniformBlockBinding);
#else
		::glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
#endif
	}

	GLenum glCheckFramebufferStatus(GLenum target)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		return glCheckFramebufferStatus_(target);
#else
		return ::glCheckFramebufferStatus(target);
#endif
	}

	void glDrawBuffers(GLsizei n, const GLenum* bufs)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glDrawBuffers_(n, bufs);
#else
		::glDrawBuffers(n, bufs);
#endif
	}

	void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glBlendFuncSeparate_(srcRGB, dstRGB, srcAlpha, dstAlpha);
#else
		::glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
#endif
	}

	void glBlendEquation(GLenum mode)
	{
#ifndef MHE_USE_NATIVE_OPENGL
		glBlendEquation_(mode);
#else
		::glBlendEquation(mode);
#endif
	}
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
    PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv_;
    PFNGLUNIFORM3FVPROC glUniform3fv_;
    PFNGLUNIFORM4FVPROC glUniform4fv_;
	PFNGLUNIFORM1IPROC glUniform1i_;
    PFNGLUNIFORM1FPROC glUniform1f_;
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
	PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform_;
	PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers_;
	PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer_;
	PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture_;
	PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex_;
	PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv_;
	PFNGLGETUNIFORMINDICESPROC glGetUniformIndices_;
	PFNGLGETACTIVEUNIFORMSIVPROC glGetActiveUniformsiv_;
	PFNGLBINDBUFFERBASEPROC glBindBufferBase_;
	PFNGLBUFFERSUBDATAPROC glBufferSubData_;
	PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding_;
	PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus_;
	PFNGLDRAWBUFFERSPROC glDrawBuffers_;
	PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate_;
	PFNGLBLENDEQUATIONPROC glBlendEquation_;
#endif
	std::map<std::string, bool> loaded_extensions_;
};

}

#endif
