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

template <class V, class I>
void caculate_normals(V* vertices, const I* indices, size_t indices_number)
{
	ASSERT(indices_number % 3 == 0, "Only triangulated meshes are currently supported");
	for (size_t i = 0; i < indices_number; i += 3)
	{
		V& v1 = vertices[indices[i + 0]];
		V& v2 = vertices[indices[i + 1]];
		V& v3 = vertices[indices[i + 2]];

		vec3 v2v1 = v2.pos - v1.pos;
		vec3 v3v1 = v3.pos - v1.pos;
		vec3 v3v2 = v3.pos - v2.pos;

		v1.nrm += cross(v2v1, v3v1).normalized();
		v2.nrm += cross(v3v2, -v2v1).normalized();
		v3.nrm += cross(-v3v1, -v3v2).normalized();

		v1.nrm.normalize();
		v2.nrm.normalize();
		v3.nrm.normalize();
	}
}

}

#endif
