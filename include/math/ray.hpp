﻿#ifndef __RAY_HPP__
#define __RAY_HPP__

#include "geometry.hpp"
#include "aabb.hpp"
#include "matrix.hpp"
#include "arch/arch_math.hpp"

namespace mhe {

template <class T>
struct ray
{
	vector3<T> origin;
	vector3<T> direction;
	float length;

	ray() : length(0) {}

	ray(const vec3& origin_value, const vec3& dst) :
		origin(origin_value), direction((dst - origin_value).normalized()), length((dst - origin_value).length())
	{}

	ray(const vec3& origin_value, const vec3& dir, T ray_length) :
		origin(origin_value), direction(dir), length(ray_length)
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

template <class T>
ray<T> operator* (const ray<T>& r, const matrix<T>& m)
{
	ray<T> res;
	res.origin = r.origin * m;
	res.direction = (vector4<T>(r.direction, 0.0f) * m).xyz();
	res.length = r.length;
	return res;
}

// intersections

template <class T>
bool intersects_moller_trumbore(hit<T>& h, const ray<T>& r, const Triangle<T>& tri)
{
	h.intersects = false;

	const vector3<T>& ac = tri.vertices[0] - tri.vertices[2];
	const vector3<T>& bc = tri.vertices[1] - tri.vertices[2];
	const vector3<T>& d = -r.direction;

	const vector3<T>& normal = vec_cross(ac, bc);
	T det = vec_dot(d, normal);
	if (det < fp_epsilon) return false;

	T inv_det = 1 / det;
	const vector3<T>& oc = r.origin - tri.vertices[2];
	T u = vec_dot(vec_cross(oc, bc), d) * inv_det;
	if (u < 0 || u > 1) return false;

	T v = vec_dot(vec_cross(ac, oc), d) * inv_det;
	if (v < 0 || v > 1) return false;

	if (u + v > 1) return false;

	T t = vec_dot(normal, oc) * inv_det;
	h.distance = t;
	h.point = r.origin + t * r.direction;
	h.intersects = true;
	h.normal = normal.normalized();
	return true;
}

// TODO: use Möller-Trumbore algorithm here
template <class T>
bool intersects(hit<T>& h, const ray<T>& r, const Triangle<T>& tri)
{
	return intersects_moller_trumbore(h, r, tri);

	h.intersects = false;
	const vector3<T>& v1 = tri.vertices[1] - tri.vertices[0];
	const vector3<T>& v2 = tri.vertices[2] - tri.vertices[0];

	vector3<T> normal = vec_cross(v1, v2);

	// backface culling
	if (vec_dot(normal.normalized(), r.direction) >= -fp_epsilon) return false;

	T area = normal.magnitude();
	T t = (vec_dot(tri.vertices[0], normal) - vec_dot(r.origin, normal)) / vec_dot(r.direction, normal);
	if (t < 0.0f) return false;

	h.point = r.origin + t * r.direction;
	h.distance = t;

	// now we check that res is inside the triangle
	vec3 resv0 = h.point - tri.vertices[0];
	vec3 v1v0 = v1;
	vector3<T> subtri_normal = vec_cross(v1v0, resv0);
	T u = subtri_normal.magnitude() / area;
	if (vec_dot(normal, subtri_normal) < -fp_epsilon) return false;	
	if (u < 0.0f || u > 1.0f) return false;
	vec3 v2v1 = tri.vertices[2] - tri.vertices[1];
	vec3 resv1 = h.point - tri.vertices[1];
	subtri_normal = vec_cross(v2v1, resv1);
	T v = subtri_normal.magnitude() / area;
	if (vec_dot(normal, subtri_normal) < -fp_epsilon) return false;
	if (v < 0.0f || v > 1.0f) return false;
	T w = 1.0f - u - v;
	vec3 resv2 = h.point - tri.vertices[2];
	vec3 v0v2 = tri.vertices[0] - tri.vertices[2];
	subtri_normal = vec_cross(v0v2, resv2);
	if (vec_dot(normal, subtri_normal) < -fp_epsilon) return false;
	if (w < 0.0f || w > 1.0f) return false;

	h.intersects = true;
	h.normal = normal.normalized();
	return true;
}

// http://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
// http://www.cs.utah.edu/~awilliam/box/box.pdf
template <class T>
bool intersects(vector3<T>& input, vector3<T>& output, const ray<T>& r, const AABB<T>& aabb)
{
	NOT_IMPLEMENTED(input);
	NOT_IMPLEMENTED(output);
	vector3<T> bounds[2];
	aabb.min_max(bounds[0], bounds[1]);

	vector3<T> invdir = (T)1 / (r.direction * r.length);
	int sign[3] = {invdir.x() < 0, invdir.y() < 0, invdir.z() < 0};

	T tmin = (bounds[sign[0]].x() - r.origin.x()) * invdir.x();
	T tmax = (bounds[1 - sign[0]].x() - r.origin.x()) * invdir.x();
	T tymin = (bounds[sign[1]].y() - r.origin.y()) * invdir.y();
	T tymax = (bounds[1 - sign[1]].y() - r.origin.y()) * invdir.y();

	if (tmin > tymax || tymin > tmax) return false;
	if (tymin > tmin) tmin = tymin;
	if (tymax < tmax) tmax = tymax;

	T tzmin = (bounds[sign[2]].z() - r.origin.z()) * invdir.z();
	T tzmax = (bounds[1 - sign[2]].z() - r.origin.z()) * invdir.z();

	if (tmin > tzmax || tzmin > tmax) return false;
	if (tzmin > tmin) tmin = tzmin;
	if (tzmax < tmax) tmax = tzmax;

	return true;
}

}

#endif
