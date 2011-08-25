#include "impl/system_factory.hpp"

#ifdef __INCLUDE_SDL__
    #include "sdl_input.hpp"
	#include "sdl_window_system.hpp"
#endif
/*

#ifdef __INCLUDE_OGLFT__
    #include "gui/oglft_font.hpp"
#endif

#ifdef __INCLUDE_MHE_GUI__
    #include "gui/mhe_label.hpp"
#endif
*/
#include "texture.hpp"
#include "sound/openal_sound.hpp"

namespace mhe {

iTexture* SystemFactory::createTexture()
{
	return new Texture();
}

iSound* SystemFactory::createSound()
{
	return new OpenALSound();
}

iWindowSystem* SystemFactory::createWindowSystem()
{
    #ifdef __INCLUDE_SDL__
    return new SDLWindowSystem;
    #endif

    return nullptr;
}	

iInputSystem* SystemFactory::createInputSystem()
{
    #ifdef __INCLUDE_SDL__
    return new SDLInputSystem;
    #endif

    return nullptr;
}
/*

namespace gui {
iFontManager* GUIFactory::createFontManager()
{
    #ifdef __INCLUDE_OGLFT__
    return new gui::oglftFontManager;
    #endif

    return nullptr;
}

LabelImpl* GUIFactory::createLabel()
{
    #ifdef __INCLUDE_MHE_GUI__
    return new mheLabel;
    #endif

    return nullptr;
}

};  // gui*/

};  // mhe
