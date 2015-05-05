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

	void min_max(vector3<T>& min_pos, vector3<T>& max_pos) const
	{
		min_pos = center - extents;
		max_pos = center + extents;
	}

	static AABB from_min_max(const vector3<T>& min_pos, const vector3<T>& max_pos)
	{
		return AABB<T>((max_pos + min_pos) * 0.5f, (max_pos - min_pos) * 0.5f);
	}
};

template <class T>
AABB<T> max(const AABB<T>& aabb1, const AABB<T>& aabb2)
{
	vector3<T> min_pos = min(aabb1.center - aabb1.extents, aabb2.center - aabb2.extents);
	vector3<T> max_pos = max(aabb1.center + aabb1.extents, aabb2.center + aabb2.extents);
	return AABB<T>::from_min_max(min_pos, max_pos);
}

template <class T>
inline std::ostream& operator<< (std::ostream& s, const AABB<T>& aabb)
{
	return s << "c:" << aabb.center << " e:" << aabb.extents;
}

typedef AABB<float> AABBf;

}

#endif
