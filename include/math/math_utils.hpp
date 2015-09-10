#ifndef _MATH_UTILS_HPP_
#define _MATH_UTILS_HPP_

#include "core/types.hpp"
#include <cfloat>

namespace mhe
{
    const float pi = 3.14159265359f;
	const float pi_2 = pi * 0.5f;
    const float deg2rad = pi / 180.0f;
    const float rad2deg = 180.0f / pi;
	const float float_max = FLT_MAX;
	const float float_min = FLT_MIN;
	const float fp_epsilon = 0.00001f;

    inline float deg_to_rad(float deg)
    {
        return deg * deg2rad;
    }

    inline float rad_to_deg(float rad)
	{
		return rad * rad2deg;
	}

	inline bool is_pot(uint num)
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

template <class T>
inline T lerp(const T& begin, const T& end, float value)
{
	return begin + (end - begin) * value;
}

template <class T>
inline T clamp(const T& value, const T& min, const T& max)
{
	if (value > max) return max;
	if (value < min) return min;
	return value;
}

template <class T>
inline T clamp_up(const T& value, const T& max)
{
	if (value > max) return max;
	return value;
}

};

#endif
