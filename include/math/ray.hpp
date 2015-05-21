#ifndef __RAY_HPP__
#define __RAY_HPP__

#include "geometry.hpp"
#include "aabb.hpp"

namespace mhe {

template <class T>
struct ray
{
	vector3<T> origin;
	vector3<T> direction;
	float length;

	ray(const vec3& origin_value, const vec3& dst) :
		origin(origin_value), direction((dst - origin_value).normalized()), length((dst - origin_value).length())
	{}

	ray(const vec3& origin_value, const vec3& dir, T length) :
		origin(origin_value), direction(dir), length(1.0f)
	{}
};

typedef ray<float> rayf;

// intersections
template <class T>
bool intersects(vector3<T>& res, float& dist, const ray<T>& r, const Triangle<T>& tri)
{
	const vector3<T>& v1 = tri.vertices[1] - tri.vertices[0];
	const vector3<T>& v2 = tri.vertices[2] - tri.vertices[0];

	vector3<T> normal = cross(v1, v2);
	if (dot(r.direction, normal.normalized()) > 0.0f) return false;

	T area = normal.magnitude2();
	T t = (dot(tri.vertices[0], normal) - dot(r.origin, normal)) / dot(r.direction, normal);
	if (t < 0.0f) return false;

	res = r.origin + t * r.direction;

	// now we check that res is inside the triangle
	vec3 resv0 = res - tri.vertices[0];
	vec3 v1v0 = v1;
	T u = cross(resv0, v1v0).magnitude2() / area;
	if (u < 0.0f || u > 1.0f) return false;
	vec3 v2v0 = v2;
	T v = cross(resv0, v2v0).magnitude2() / area;
	if (v < 0.0f || v > 1.0f) return false;
	T w = 1.0f - u - v;
	if (w < 0.0f || w > 1.0f) return false;

	return true;
}

template <class T>
bool intersects(vector3<T>& input, vector3<T>& output, const ray<T>& r, const AABB<T>& aabb)
{
	return false;
}

}

#endif
