#include "impl/system_factory.hpp"
#include "platform/platform.hpp"

#include "video_driver.hpp"
#include "texture.hpp"
#include "sound/isound.hpp"
#include "sound/iaudio_driver.hpp"

namespace mhe {

Driver* SystemFactory::create_driver() const
{
#ifdef MHE_OPENGL
	return new opengl::OpenGLDriver();
#endif
}

Texture* SystemFactory::create_texture() const
{
#ifdef MHE_OPENGL
	return new opengl::OpenGLTexture();
#endif
	return nullptr;
}

Texture* SystemFactory::create_multitexture() const
{
#ifdef MHE_OPENGL
	return new opengl::OpenGLMultiTexture();
#endif
	return nullptr;
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

iWindowSystem* SystemFactory::create_window_system() const
{
	// TODO:
	/*
	if (factory_ != nullptr) return factory_->create_window_system();
    #ifdef MHE_SDL
    return new sdl::SDLWindowSystem;
    #endif
	*/

    return nullptr;
}	

}  // mhe
