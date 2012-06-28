#include "impl/system_factory.hpp"
#include "platform/platform.hpp"

#include "sound/isound.hpp"
#include "sound/iaudio_driver.hpp"

namespace mhe {

Driver* SystemFactory::create_driver() const
{
	return video_driver_factory_.create_video_driver();
}

Texture* SystemFactory::create_texture() const
{
	return video_driver_factory_.create_texture();
}

iAudioDriver* SystemFactory::create_audio_driver() const
{
#ifdef MHE_OPENAL
	return new OpenALAudioDriver();
#endif
	return nullptr;
}

iSound* SystemFactory::create_sound() const
{
#ifdef MHE_OPENAL
	return new OpenALSound();
#endif
	return nullptr;
}

WindowSystemImpl* SystemFactory::create_window_system() const
{
	return window_system_factory_.create_window_system();
}	

}  // mhe
