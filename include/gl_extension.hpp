#ifndef _GL_EXTENSION_HPP_
#define _GL_EXTENSION_HPP_

#include <GL/gl.h>
//#include <GL/glext.h>
#include "glext.h"
#include <map>
#include <string>

//#ifdef __INCLUDE_SDL__
#include <SDL/SDL.h>
//#endif

#ifdef __WIN32__
    extern PFNGLMULTITEXCOORD2FPROC glMultiTexCoord2f;
    extern PFNGLACTIVETEXTUREPROC glActiveTexture;
    extern PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;

    extern PFNGLCOMPILESHADERPROC glCompileShader;
#endif

namespace mhe
{
    void load_default_extensions();
    void* load_extension(const std::string& ext);
    bool is_extension_present(const std::string& ext);
	std::string get_availible_extensions();
	bool is_extension_availible(const char* ext);
};

#endif
