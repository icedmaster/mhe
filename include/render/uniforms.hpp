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
	mat4x4 lightvp;
	vec4 shadowmap_params;
};

const size_t perframe_data_unit = 0;
const size_t permodel_data_unit = 1;

// Textures
const size_t albedo_texture_unit = 0;
const size_t normal_texture_unit = 2;
const size_t shadowmap_texture_unit = 5;

}

#endif
