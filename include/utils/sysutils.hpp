#ifndef _SYSUTILS_HPP_
#define _SYSUTILS_HPP_

#include <ctime>
#include <cstdlib>
#include "platform/platform_system.hpp"

namespace mhe
{
    namespace utils
    {
        inline void init_randomizer()
        {
            srand(time(0));
        }

        inline uint random(uint limit)
        {
            return (rand() % limit);
        }

        // with sign
        inline int srandom(uint limit, int probability = 50)
        {
            int n = random(limit);
            // get random sign
            int s = random(100);
            if (s < probability) return n;
            return -n;
        }

		inline uint get_current_tick()
		{
			return impl::get_current_tick();
		}

		inline float get_current_time()
		{
			return impl::get_current_time();
		}

		inline float get_last_delta()
		{
			return impl::get_last_delta();
		}

    }
}

#endif
