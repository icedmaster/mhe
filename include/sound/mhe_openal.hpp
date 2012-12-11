#ifndef __MHE_OPENAL_HPP__
#define __MHE_OPENAL_HPP__

#include "config.hpp"

#if defined(MHE_MACOS)
#include <al.h>
#include <alc.h>
#elif defined(MHE_IOS)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#endif
