#ifndef _MHE_GL_HPP_
#define _MHE_GL_HPP_

#include "config.hpp"

#ifdef MHE_IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#elif defined(MHE_MACOS)
#define GL_GLEXT_PROTOTYPES
#define MHE_OPENGL3
#include <OpenGL/gl.h>
#include "glext.h"
#else
	#ifdef MHE_WIN
		#include <Windows.h>
	#endif
    #include <GL/gl.h>
    #include "glext.h"
#endif

#ifndef MHE_WIN
	#define MHE_USE_NATIVE_OPENGL
#endif

#if defined(MHE_OPENGLES) || defined(MHE_OPENGL3)
#define MHE_OPENGL_HAS_SHADERS
#endif

#endif
