#ifndef __UNIFORMS_HPP__
#define __UNIFORMS_HPP__

#include "math/matrix.hpp"

namespace mhe {

struct TransformSimpleData
{
	mat4x4 vp;
};

struct TransformData
{
	mat4x4 vp;
	mat4x4 inv_proj;
	mat4x4 inv_vp;
	vec4 viewpos;
};

struct PerModelSimpleData
{
	mat4x4 model;
};

struct PerModelData
{
	mat4x4 model;
	mat4x4 normal;
};

struct LightData
{
	vec4 diffuse;
	vec4 specular;
	vec4 position;
	vec4 direction;
};

}

#endif
