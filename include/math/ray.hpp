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

template <class T>
struct hit
{
	vector3<T> point;
	vector3<T> normal;
	float distance;
	bool intersects;
};

typedef ray<float> rayf;
typedef hit<float> hitf;

// intersections
template <class T>
bool intersects(hit<T>& h, const ray<T>& r, const Triangle<T>& tri)
{
	h.intersects = false;
	const vector3<T>& v1 = tri.vertices[1] - tri.vertices[0];
	const vector3<T>& v2 = tri.vertices[2] - tri.vertices[0];

	vector3<T> normal = cross(v1, v2);

	// backface culling
	if (dot(normal.normalized(), r.direction) >= -fp_epsilon) return false;

	T area = normal.magnitude();
	T t = (dot(tri.vertices[0], normal) - dot(r.origin, normal)) / dot(r.direction, normal);
	if (t < 0.0f) return false;

	h.point = r.origin + t * r.direction;
	h.distance = t;

	// now we check that res is inside the triangle
	vec3 resv0 = h.point - tri.vertices[0];
	vec3 v1v0 = v1;
	vector3<T> subtri_normal = cross(v1v0, resv0);
	T u = subtri_normal.magnitude() / area;
	if (dot(normal, subtri_normal) < -fp_epsilon) return false;	
	if (u < 0.0f || u > 1.0f) return false;
	vec3 v2v1 = tri.vertices[2] - tri.vertices[1];
	vec3 resv1 = h.point - tri.vertices[1];
	subtri_normal = cross(v2v1, resv1);
	T v = subtri_normal.magnitude() / area;
	if (dot(normal, subtri_normal) < -fp_epsilon) return false;
	if (v < 0.0f || v > 1.0f) return false;
	T w = 1.0f - u - v;
	vec3 resv2 = h.point - tri.vertices[2];
	vec3 v0v2 = tri.vertices[0] - tri.vertices[2];
	subtri_normal = cross(v0v2, resv2);
	if (dot(normal, subtri_normal) < -fp_epsilon) return false;
	if (w < 0.0f || w > 1.0f) return false;

	h.intersects = true;
	h.normal = subtri_normal.normalized();
	return true;
}

template <class T>
bool intersects(vector3<T>& input, vector3<T>& output, const ray<T>& r, const AABB<T>& aabb)
{
	return false;
}

}

#endif
