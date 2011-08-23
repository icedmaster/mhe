#ifndef _OPENAL_SOUND_HPP_
#define _OPENAL_SOUND_HPP_

#include "isound.hpp"
#include <AL/al.h>

namespace mhe
{
	class OpenALSound : public iSound
	{
		private:
			ALuint id_;
			ALuint src_id_;
		public:
			OpenALSound();
			~OpenALSound();
			
			bool init(boost::shared_ptr<iSoundData> data);
			
			void play();
			void stop();
	};
}

#endif
