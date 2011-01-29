#include "impl/system_factory.hpp"

#ifdef __INCLUDE_SDL__
    #include "sdl_input.hpp"
#endif

#ifdef __INCLUDE_OGLFT__
    #include "gui/oglft_font.hpp"
#endif

#ifdef __INCLUDE_MHE_GUI__
    #include "gui/mhe_label.hpp"
#endif

namespace mhe {

iInputSystem* SystemFactory::createInputSystem()
{
    #ifdef __INCLUDE_SDL__
    return new SDLInputSystem;
    #endif

    return nullptr;
}

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

};  // gui

};  // mhe
