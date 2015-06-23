#ifndef __UNIFORMS_HPP__
#define __UNIFORMS_HPP__

#include "math/matrix.hpp"
#include "core/config.hpp"

namespace mhe {

struct TransformSimpleData
{
	mat4x4 vp;
};

struct PerCameraData
{
	mat4x4 vp;
	mat4x4 inv_vp;
	mat4x4 inv_proj;
	vec4 viewpos;
	vec4 ambient;
	float znear;
	float zfar;
	vec2 inv_viewport;
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
    mat4x4 lightw;
	vec4 shadowmap_params;
};

struct DirectionalLightData
{
	vec4 diffuse;
	vec4 specular;
	vec4 direction;
	mat4x4 lightvp;
	vec4 csm_scale[max_shadowmap_cascades_number];
	vec4 csm_offset[max_shadowmap_cascades_number];
	float cascade_znear[max_shadowmap_cascades_number];
	float cascade_zfar[max_shadowmap_cascades_number];
	vec4 shadowmap_params;
	uint32_t cascades_number;
	uint32_t padding[3];
};

struct PhongMaterialData
{
	vec4 diffuse;
	vec4 specular;
	vec4 params;
};

const size_t perframe_data_unit = 0;
const size_t permodel_data_unit = 1;
const size_t material_data_unit = 2;

// Textures
const size_t albedo_texture_unit = 0;
const size_t normal_texture_unit = 2;
const size_t shadowmap_texture_unit = 5;
const size_t env_cubemap_texture_unit = 6;

}

#endif
