#ifndef _MATH_UTILS_HPP_
#define _MATH_UTILS_HPP_

#include "types.hpp"

namespace mhe
{
    const float pi = 3.14159265359f;
    const float deg2rad = pi / 180.0;
    const float rad2deg = 180.0 / pi;

    inline float deg_to_rad(float deg)
    {
        return deg * deg2rad;
    }

	inline bool is_pot(cmn::uint num)
	{
		return !(num & (num - 1));
	}

	inline uint32_t nearest_pot(uint32_t x)
	{
		x = x - 1;
		x = x | (x >> 1);	
		x = x | (x >> 2);
		x = x | (x >> 4);
		x = x | (x >> 8);
		x = x | (x >> 16);
		return x + 1;
	}	
};

#endif
