#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include "itimer.hpp"
#include <SDL/SDL_timer.h>

namespace mhe
{
    class Timer : public iTimer
    {
        private:
            cmn::uint end_count_;
            cmn::uint interval_;
            mutable bool is_running_;
        private:
            void set_impl(cmn::uint interval)
            {
                interval_ = interval;
            }

            void start_impl()
            {
                end_count_ = SDL_GetTicks() + interval_;
                is_running_ = true;
            }

            void stop_impl()
            {
                is_running_ = false;
                end_count_ = 0;
            }

            bool is_elapsed() const
            {
                if ( is_running_ && (SDL_GetTicks() >= end_count_) )
                {
                    is_running_ = false;
                    return true;
                }
                return false;
            }
    };
};

#endif
