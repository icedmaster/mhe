#ifndef __NODE_HPP__
#define __NODE_HPP__

#include "mesh.hpp"
#include "material.hpp"
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
	POOL_STRUCT(uint16_t);
	RenderCommand* command;
	RenderState::IdType state;
	RenderTarget::IdType render_target;

	DrawCallData() :
		id(invalid_id),
		command(nullptr),
		state(RenderState::invalid_id),
		render_target(default_render_target)
	{}
};

struct RenderPassData
{
	uint16_t draw_call_data;
	Material::IdType material;
};

struct MainPassData
{
	uint16_t draw_call_data;
	MaterialInstance material;
};

struct AdditionalPasses
{
	POOL_STRUCT(uint16_t);
	typedef fixed_size_vector<RenderPassData, max_additional_render_passes> Passes;
	Passes passes;
};

struct Node
{
	Mesh mesh;
	MainPassData main_pass;
	AdditionalPasses::IdType additional_passes;
};

}

#endif
