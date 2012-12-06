#ifndef _MHE_GL_HPP_
#define _MHE_GL_HPP_

#include "config.hpp"

#ifdef MHE_IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#elif defined(MHE_MACOS)
#define GL_GLEXT_LEGACY
#include <OpenGL/gl.h>
#include "glext.h"
#else
    #include <GL/gl.h>
    #include "glext.h"
#endif

#endif
