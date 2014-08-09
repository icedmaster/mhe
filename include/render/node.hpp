#ifndef __NODE_HPP__
#define __NODE_HPP__

#include "mesh.hpp"
#include "material.hpp"
#include "transform.hpp"
#include "render_state.hpp"
#include "render_target.hpp"

namespace mhe {

class RenderCommand
{
public:
	virtual ~RenderCommand() {}
	virtual bool execute() = 0;
};

struct DrawCallData
{
	RenderCommand* command;
	RenderState::IdType state;
	RenderTarget::IdType render_target;
	uint16_t id;

	DrawCallData() :
		command(nullptr),
		render_target(default_render_target)
	{}
};

struct Node
{
	Mesh mesh;
	MaterialInstance material;
	Transform::IdType transform;
	uint16_t draw_call_data;
	uint16_t id;
};

}

#endif
