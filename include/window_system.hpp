#ifndef _WINDOW_SYSTEM_HPP_
#define _WINDOW_SYSTEM_HPP_

#include "globals.hpp"
#include <SDL/SDL.h>

namespace mhe
{
    class WindowSystem
    {
        public:
            cmn::uint width() const
            {
                return globals::instance().window_width();
            }

            cmn::uint height() const
            {
                return globals::instance().window_height();
            }

            void swapBuffers()
            {
                SDL_GL_SwapBuffers();
            }
    };
};

#endif
