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

        inline cmn::uint random(cmn::uint limit)
        {
            return (rand() % limit);
        }

		inline cmn::uint range_random(cmn::uint min, cmn::uint max)
		{
			if (min == max) return min;
			return (rand() % (max - min) + min);
		}

		inline float range_random(float min, float max)
		{
			if (min == max) return min;
			return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (max - min) + min;
		}

        // with sign
        inline int srandom(cmn::uint limit, int probability = 50)
        {
            int n = random(limit);
            // get random sign
            int s = random(100);
            if (s < probability) return n;
            return -n;
        }

		inline cmn::uint get_current_tick()
		{
			return impl::get_current_tick();
		}			
    }
}

#endif
