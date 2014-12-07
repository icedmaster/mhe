#ifndef __AABB_HPP__
#define __AABB_HPP__

#include "vector3.hpp"

namespace mhe {

template <class T>
struct AABB
{
    vector3<T> center;
    vector3<T> extents;

	AABB() {}
	
	template <class U>
    AABB(const vector3<U>& center_value, const vector3<U>& extents_value) :
        center(center_value), extents(extents_value)
	{}
};

typedef AABB<float> AABBf;

}

#endif
