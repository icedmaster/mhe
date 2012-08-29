#ifndef _MHE_GL_HPP_
#define _MHE_GL_HPP_

#include "config.hpp"

#ifndef MHE_MACOS
    #include <GL/gl.h>
#else
    #define GL_GLEXT_LEGACY
    #include <OpenGL/gl.h>
#endif

#endif
