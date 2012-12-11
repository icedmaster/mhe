#ifndef __OPENGL_UTILS_HPP__
#define __OPENGL_UTILS_HPP__

#include "mhe_gl.hpp"
#include "utils/global_log.hpp"

namespace mhe {
namespace opengl {

inline void check_for_errors()
{
	int error = static_cast<int>(glGetError());
	if (error != GL_NO_ERROR)
		WARN_LOG("OpenGL error:" << error);
}

}}

#endif
