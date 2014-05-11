#ifndef __PLATFORM_HPP__
#define __PLATFORM_HPP__

#include "core/config.hpp"

#ifdef MHE_OPENGL3
#include "platform/opengl3/opengl3_platform.hpp"
#endif

#ifdef MHE_OPENGLES
#include "platform/opengles/opengles_platform.hpp"
#endif

#ifdef MHE_SDL2
#include "platform/sdl2/sdl2_platform.hpp"
#endif

#ifdef MHE_QT
#include "platform/qt/qt_platform.hpp"
#endif

#endif
