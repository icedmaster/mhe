#ifndef _OPENAL_AUDIO_DRIVER_HPP_
#define _OPENAL_AUDIO_DRIVER_HPP_

#include "iaudio_driver.hpp"
#include "utils/logutils.hpp"
#include <al.h>
#include <alc.h>

namespace mhe
{
	class OpenALAudioDriver : public iAudioDriver
	{
		private:
			ALCdevice* device_;
			ALCcontext* context_;
		public:
			bool init();
	};
}

#endif
