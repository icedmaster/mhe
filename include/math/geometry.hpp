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

}

#endif
