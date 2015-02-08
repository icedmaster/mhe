#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "render_buffer.hpp"
#include "render_state.hpp"
#include "material.hpp"
#include "render_target.hpp"

namespace mhe {

enum Primitive
{
	triangle = 0,
	lines = 1
};

struct DrawCallData
{
    POOL_STRUCT(uint16_t);
    RenderState::IdType state;
    RenderTarget::IdType render_target;

    DrawCallData() :
        id(invalid_id),
        state(RenderState::invalid_id),
        render_target(default_render_target)
    {}
};

struct RenderData
{
	size_t elements_number;
	uint16_t vbuffer_offset;
	uint16_t ibuffer_offset;
	Primitive primitive;
    Layout::IdType layout;
    VertexBuffer::IdType vbuffer;
    IndexBuffer::IdType ibuffer;

	RenderData() :
        vbuffer_offset(0), ibuffer_offset(0), primitive(triangle),
        layout(Layout::invalid_id), vbuffer(VertexBuffer::invalid_id), ibuffer(IndexBuffer::invalid_id)
	{}
};

struct MeshPart
{
    RenderData render_data;
};

struct Mesh
{
    std::vector<MeshPart> parts;
    VertexBuffer::IdType vbuffer;
    IndexBuffer::IdType ibuffer;

    Mesh() : vbuffer(VertexBuffer::invalid_id), ibuffer(IndexBuffer::invalid_id)
    {}
};

inline MeshPart& add_part(Mesh& mesh)
{
    MeshPart part;
    part.render_data.ibuffer = mesh.ibuffer;
    part.render_data.vbuffer = mesh.vbuffer;
    mesh.parts.push_back(part);
    return mesh.parts.back();
}

struct MeshPartInstance
{
    MaterialInstance material;
    DrawCallData::IdType draw_call_data;
};

struct MeshInstance
{
    Mesh mesh;
    std::vector<MeshPartInstance> instance_parts;
    UniformBuffer::IdType shared_uniform;

    MeshInstance() : shared_uniform(UniformBuffer::invalid_id) {}
};

inline MeshPartInstance& add_part(MeshInstance& mesh)
{
    add_part(mesh.mesh);
    mesh.instance_parts.resize(mesh.instance_parts.size() + 1);
    return mesh.instance_parts.back();
}

}

#endif
