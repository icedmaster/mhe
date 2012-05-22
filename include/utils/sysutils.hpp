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
