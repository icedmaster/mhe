#ifndef __PLATFORM_HPP__
#define __PLATFORM_HPP__

#include "config.hpp"

#ifdef MHE_OPENGL
#include "platform/opengl/opengl_platform.hpp"
#endif

#ifdef MHE_OPENGL3
#include "platform/opengl3/opengl3_platform.hpp"
#endif

#ifdef MHE_OPENGLES
#include "platform/opengles/opengles_platform.hpp"
#endif

#ifdef MHE_SDL
#include "platform/sdl/sdl_platform.hpp"
#endif

#ifdef MHE_QT
#include "platform/qt/qt_platform.hpp"
#endif

#ifdef MHE_OPENAL
#include "platform/openal/openal_audio_driver.hpp"
#include "platform/openal/openal_sound.hpp"
#endif

#ifdef MHE_IOS
#include "platform/ios/ios_platform.hpp"
#endif

#endif
