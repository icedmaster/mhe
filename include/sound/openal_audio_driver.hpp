#ifndef _OPENAL_AUDIO_DRIVER_HPP_
#define _OPENAL_AUDIO_DRIVER_HPP_

#include "iaudio_driver.hpp"
#include "utils/logutils.hpp"
#include <AL/al.h>
#include <AL/alc.h>

namespace mhe
{
	class OpenALAudioDriver : public iAudioDriver
	{
		private:
			ALCdevice* device_;
			ALCcontext* context_;
		public:
			OpenALAudioDriver();
			~OpenALAudioDriver();
			bool init();
	};
}

#endif