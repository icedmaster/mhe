#include "sound/openal_audio_driver.hpp"

namespace mhe
{
	OpenALAudioDriver::OpenALAudioDriver() :
		device_(0),
		context_(0)
	{}
	
	OpenALAudioDriver::~OpenALAudioDriver()
	{
		alcMakeContextCurrent(0);
		if (context_)
			alcDestroyContext(context_);
		if (device_)
			alcCloseDevice(device_);
	}
	
	bool OpenALAudioDriver::init()
	{
		device_  = alcOpenDevice(0);
		context_ = alcCreateContext(device_, 0);
		alcMakeContextCurrent(context_);
				
		if (alGetError() != AL_NO_ERROR)
		{
			utils::global_log::instance().write("OpenAL initialization failed");
			return false;
		}
		utils::global_log::instance().write("OpenAL initialization successfull");
		return true;
	}
}
			