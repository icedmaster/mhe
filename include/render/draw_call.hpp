#ifndef __DRAW_CALL_HPP__
#define __DRAW_CALL_HPP__

#include "node.hpp"

namespace mhe {

struct DrawCall
{
	DrawCallData::IdType draw_call_data;
	MaterialInstance material;
	RenderData render_data;
	RenderCommand* command;
	uint8_t pass;

	DrawCall() : draw_call_data(DrawCallData::invalid_id), command(nullptr), pass(0) {}
};

struct DrawCallExplicit
{
	RenderState* render_state;
	RenderTarget* render_target;
	Texture* textures[material_textures_number];
	UniformBuffer* uniforms[material_uniforms_number];
	TextureBuffer* texture_buffers[material_texture_buffers_number];
	ShaderProgram* shader_program;
	size_t elements_number;
	uint32_t vbuffer_offset;
	uint32_t ibuffer_offset;
	size_t indices_number;
	Primitive primitive;
	Layout* layout;
	IndexBuffer* ibuffer;
	VertexBuffer* vbuffer;
	RenderCommand* render_command;
	uint8_t pass;
	// Will be moved to the ID
	uint8_t priority;
};

inline void prepare_draw_call(DrawCallExplicit& draw_call)
{
	::memset(&draw_call, 0, sizeof(DrawCallExplicit));
}

}

#endif // DRAW_CALL_HPP
