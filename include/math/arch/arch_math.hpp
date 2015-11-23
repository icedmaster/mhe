#ifndef __ARCH_MATH_HPP__
#define __ARCH_MATH_HPP__

#include "core/config.hpp"

#include "sse/sse_math.hpp"
#include "../vector3.hpp"

namespace mhe {

typedef details::float32x4_t float32x4_t;

inline float32x4_t vectorize(const vec3& v)
{
	float vf[4] = {v.x(), v.y(), v.z(), 0.0f};
	return details::vectorize(vf);
}

inline vec3 unvectorize(float32x4_t vv)
{
	float vf[4];
	details::unvectorize(vf, vv);
	return vec3(vf[3], vf[2], vf[1]);
}

inline float vec_dot(const vec3& v1, const vec3& v2)
{
	details::float32x4_t r = details::dot(vectorize(v1), vectorize(v2));
	float vf[4];
	details::unvectorize(vf, r);
	return vf[0];
}

inline vec3 vec_cross(const vec3& v1, const vec3& v2)
{
	float32x4_t r = details::cross(vectorize(v1), vectorize(v2));
	return unvectorize(r);
}

}

#endif
