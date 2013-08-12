#ifndef __MHE_SOUND_HPP__
#define __MHE_SOUND_HPP__

#ifdef MHE_HAS_OPENAL
	#include "platform/openal/openal_audio_driver.hpp"
	#include "platform/openal/openal_sound.hpp"
#endif
#ifdef MHE_HAS_VORBIS
#include "sound/vorbis/ogg_sound.hpp"
#endif
#include "sound/isound.hpp"
#include "sound/wav_sound.hpp"

#endif
