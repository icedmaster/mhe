#ifndef _GLOBALS_HPP_
#define _GLOBALS_HPP_

#include "types.hpp"

namespace mhe
{
    class globals
    {
        private:
            cmn::uint window_width_;
            cmn::uint window_height_;
            globals() :
                window_width_(0),
                window_height_(0)
            {
            }

            globals(const globals&) {}
            ~globals() {}
        public:
            static globals& instance()
            {
                static globals g;
                return g;
            }

            cmn::uint window_width() const
            {
                return window_width_;
            }

            cmn::uint window_height() const
            {
                return window_height_;
            }

            void set_window(cmn::uint w, cmn::uint h)
            {
                window_width_ = w;
                window_height_ = h;
            }
    };
};

#endif
