#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "render_buffer.hpp"
#include "render_state.hpp"

namespace mhe {

enum Primitive
{
	triangle = 0,
};

struct RenderData
{
	size_t elements_number;
	uint16_t vbuffer_offset;
	uint16_t ibuffer_offset;
	Primitive primitive;

	RenderData() :
		vbuffer_offset(0), ibuffer_offset(0), primitive(triangle)
	{}
};

struct Mesh
{
	VertexBuffer::IdType vbuffer;
	IndexBuffer::IdType ibuffer;
	RenderState::IdType state;
	uint16_t layout;
	RenderData render_data;
};

}

#endif
