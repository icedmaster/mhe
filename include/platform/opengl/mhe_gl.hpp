#ifndef _MHE_GL_HPP_
#define _MHE_GL_HPP_

#include "core/config.hpp"
#include "core/compiler.hpp"
#include "utils/global_log.hpp"

#ifdef MHE_IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#elif defined(MHE_MACOS)
#define GL_GLEXT_PROTOTYPES
#define MHE_OPENGL3
#include <OpenGL/gl.h>
#include "glext.h"
#else
    #define MHE_OPENGL3
    #ifdef MHE_WIN
        #include "platform/win/win_wrapper.hpp"
    #else
        #define GL_GLEXT_PROTOTYPES
    #endif
    #include <GL/gl.h>
    #include "glext.h"
#endif

#ifndef MHE_WIN
//#define MHE_USE_NATIVE_OPENGL
#endif

#define MHE_OPENGL_HAS_SHADERS
#define MHE_OPENGL_ABORT_ON_ERROR
//#define MHE_OPENGL_UBO_DOUBLEBUFFERING
//#define MHE_OPENGL_USE_SRGB

#ifdef MHE_OPENGL_ABORT_ON_ERROR
#define PRINT_ERROR(m) ASSERT(0, FUNCTION_DESCRIPTION_MACRO << ":" << e)
#else
#define PRINT_ERROR(m) WARN_LOG(FUNCTION_DESCRIPTION_MACRO << ":" << e);
#endif

#define CHECK_GL_ERRORS()                       \
    {                                           \
        GLenum e = glGetError();                    \
        if (e != GL_NO_ERROR)                       \
            PRINT_ERROR(e);                 \
    }                                           

#endif
