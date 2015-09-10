#ifndef __GEOMETRY_HPP__
#define __GEOMETRY_HPP__

#include "vector3.hpp"

namespace mhe {

template <class T>
struct Triangle
{
	vector3<T> vertices[3];
};

typedef Triangle<float> trianglef;

template <class T>
inline std::ostream& operator<< (std::ostream& s, const Triangle<T>& t)
{
	return s << '{' << t.vertices[0] << " " << t.vertices[1] << " " << t.vertices[2] << '}';
}

}

#endif
