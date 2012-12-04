#ifndef __PLATFORM_HPP__
#define __PLATFORM_HPP__

#include "config.hpp"

#ifdef MHE_OPENGL
#include "platform/opengl/opengl_platform.hpp"
#endif

#ifdef MHE_OPENGLES
#include "platform/opengles/opengles_platform.hpp"
#endif

#ifdef MHE_SDL
#include "platform/sdl/sdl_platform.hpp"
#endif

#ifdef MHE_OPENAL
#include "sound/openal_audio_driver.hpp"
#include "sound/openal_sound.hpp"
#include "sound/ogg_sound.hpp"
#endif

#ifdef MHE_IOS
#include "platform/ios/ios_platform.hpp"
#endif

#endif
