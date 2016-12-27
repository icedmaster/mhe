#ifndef __OPENGL_HELPERS_HPP__
#define __OPENGL_HELPERS_HPP__

#include "mhe_gl.hpp"
#include "core/string.hpp"

namespace mhe {
namespace opengl {

void init_text_description();
string enum_to_string(GLenum e);

#ifdef MHE_OPENGL_ABORT_ON_ERROR
#define PRINT_ERROR(m) ASSERT(0, FUNCTION_DESCRIPTION_MACRO << ":" << enum_to_string(e))
#else
#define PRINT_ERROR(m) WARN_LOG(FUNCTION_DESCRIPTION_MACRO << ":" << e);
#endif

#define CHECK_GL_ERRORS()                       \
{                                               \
    GLenum e = glGetError();                    \
    if (e != GL_NO_ERROR)                       \
        PRINT_ERROR(e);                         \
}                                           

}}

#endif
