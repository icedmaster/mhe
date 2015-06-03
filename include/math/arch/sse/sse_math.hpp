#ifndef __SSE_MATH_HPP__
#define __SSE_MATH_HPP__

#include "core/compiler.hpp"

#if defined(MHE_GCC) || defined(MHE_VS)
#include <emmintrin.h>

#define mm_mul_ps _mm_mul_ps
#define mm_hadd_ps _mm_hadd_ps
#define mm_load_ps _mm_load_ps
#define mm_store_ps _mm_store_ps
#define mm_shuffle_ps _mm_shuffle_ps
#define mm_add_ps _mm_add_ps
#define mm_sub_ps _mm_sub_ps
#define MM_SHUFFLE _MM_SHUFFLE
#endif

namespace mhe {
namespace details {

#if defined(MHE_GCC) || defined(MHE_VS)
typedef __m128 float32x4_t;
#endif

inline float32x4_t vectorize(const float* v)
{
	return mm_load_ps(v);
}

inline void unvectorize(float* v, float32x4_t r)
{
	mm_store_ps(v, r);
}

inline float32x4_t dot(float32x4_t v1, float32x4_t v2)
{
	float32x4_t r1 = mm_mul_ps(v1, v2);
	float32x4_t r2 = mm_shuffle_ps(r1, r1, MM_SHUFFLE(2, 3, 0, 1));
	float32x4_t r3 = mm_add_ps(r1, r2);
	r2 = mm_shuffle_ps(r3, r3, MM_SHUFFLE(0, 1, 2, 3));
	return mm_add_ps(r2, r3);
}

inline float32x4_t cross(float32x4_t v1, float32x4_t v2)
{
	// v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x
	float32x4_t r1 = mm_shuffle_ps(v1, v1, MM_SHUFFLE(1, 2, 0, 0));
	float32x4_t r2 = mm_shuffle_ps(v2, v2, MM_SHUFFLE(2, 0, 1, 0));
	
	float32x4_t r3 = mm_mul_ps(r1, r2);
	r1 = mm_shuffle_ps(v1, v1, MM_SHUFFLE(2, 0, 1, 0));
	r2 = mm_shuffle_ps(v2, v2, MM_SHUFFLE(1, 2, 0, 0));
	float32x4_t r4 = mm_mul_ps(r1, r2);
	return mm_sub_ps(r3, r4);
}

}}

#endif
