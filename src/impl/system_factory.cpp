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
	return new OpenALAudioDriver();
}

iSound* SystemFactory::create_sound() const
{
	return new OpenALSound();
}

iWindowSystem* SystemFactory::create_window_system() const
{
	if (factory_ != nullptr) return factory_->create_window_system();
    #ifdef MHE_SDL
    return new sdl::SDLWindowSystem;
    #endif

    return nullptr;
}	

InputSystem* SystemFactory::create_input_system() const
{
	if (factory_ != nullptr) return factory_->create_input_system();
    #ifdef MHE_SDL
    return new sdl::SDLInputSystem;
    #endif

    return nullptr;
}

}  // mhe
