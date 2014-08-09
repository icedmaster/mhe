#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "render_buffer.hpp"

namespace mhe {

enum Primitive
{
	triangle = 0,
	lines = 1
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
	RenderData render_data;
	uint16_t layout;
};

}

#endif
