#ifndef __OPENGL_EXTENSION_HPP__
#define __OPENGL_EXTENSION_HPP__

#include <string>
#include <map>
#include "mhe_gl.hpp"
#include "glext.h"

namespace mhe {
namespace opengl {
class OpenGLExtensions
{
public:
	void init_extensions();
	bool is_extension_supported(const std::string& ext_name) const;
	std::string get_supported_extensions() const;

	//
	void glActiveTexture(GLenum texture)
	{
		(*glActiveTexture_)(texture);
	}

	void clientActiveTexture(GLenum texture)
	{
		(*glClientActiveTexture_)(texture);
	}

	void glMultiTexCoord2f(GLenum target, GLfloat s, GLfloat t)
	{
		(*glMultiTexCoord2f_)(target, s, t);
	}
private:
	template <class T>
	T load_extension(const char* name)
	{
		bool loaded = true;
		T res = reinterpret_cast<T>(SDL_GL_GetProcAddress(name));
		if (res == 0) loaded = false;
		loaded_extensions_[name] = loaded;
		return res;
	}

	void get_str_extensions();

	PFNGLMULTITEXCOORD2FPROC glMultiTexCoord2f_;
    PFNGLACTIVETEXTUREPROC glActiveTexture_;
    PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture_;
	std::map<std::string, bool> loaded_extensions_;
};

}}

#endif
