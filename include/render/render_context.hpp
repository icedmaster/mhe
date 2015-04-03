#ifndef __RENDER_CONTEXT_HPP__
#define __RENDER_CONTEXT_HPP__

#include "core/config.hpp"
#include "core/fixed_size_vector.hpp"
#include "core/string.hpp"
#include "math/vector3.hpp"
#include "math/matrix.hpp"
#include "draw_call.hpp"

namespace mhe {

struct LightInstance;
struct NodeInstance;

typedef fixed_capacity_vector<DrawCall, max_scene_dips_number> DrawCalls;

struct RenderContext
{
    DrawCalls draw_calls;

	NodeInstance* nodes;
	size_t nodes_number;

	LightInstance* lights;
	size_t lights_number;

	mat4x4 view;
	mat4x4 proj;
	mat4x4 vp;
	mat4x4 inv_vp;
	vec3 viewpos;

    UniformBuffer::IdType percamera_uniform;

	uint32_t tick;
	uint32_t delta;
	float fdelta;

    RenderContext() : percamera_uniform(UniformBuffer::invalid_id) {}
};

const size_t max_textures_per_model = 8;

const size_t max_color_textures = 2;

struct ModelContext
{
    FilePath color_textures[max_color_textures];
    FilePath normal_texture;
    UniformBuffer::IdType transform_uniform;

    ModelContext() : transform_uniform(UniformBuffer::invalid_id) {}
};

}

#endif
