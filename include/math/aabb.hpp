#ifndef __AABB_HPP__
#define __AABB_HPP__

#include "vector3.hpp"

namespace mhe {

template <class T>
struct AABB
{
	vector3<T> center_;
	vector3<T> extents_;

	AABB() {}
	
	template <class U>
	AABB(const vector3<U>& center, const vector3<U>& extents) :
		center_(center), extents_(extents)
	{}
};

typedef AABB<float> AABBf;

}

#endif
