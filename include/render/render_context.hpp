#ifndef __RENDER_CONTEXT_HPP__
#define __RENDER_CONTEXT_HPP__

#include "math/vector3.hpp"
#include "math/matrix.hpp"

namespace mhe {

struct RenderContext
{
	mat4x4 view;
	mat4x4 proj;
	mat4x4 vp;
	vec3 viewpos;
};

const size_t max_textures_per_model = 8;

struct ModelContext
{
	mat4x4 model;
	std::string textures[max_textures_per_model];
};

}

#endif
