#ifndef __RENDER_CONTEXT_HPP__
#define __RENDER_CONTEXT_HPP__

#include "core/config.hpp"
#include "math/vector3.hpp"
#include "math/matrix.hpp"

namespace mhe {

struct LightInstance;

struct RenderContext
{
	LightInstance* lights;
	size_t lights_number;

	mat4x4 view;
	mat4x4 proj;
	mat4x4 vp;
	mat4x4 inv_vp;
	vec3 viewpos;

	uint32_t tick;
	uint32_t delta;
	float fdelta;
};

const size_t max_textures_per_model = 8;

struct ModelContext
{
	std::string textures[max_textures_per_model];
};

}

#endif
