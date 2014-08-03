#include "render/utils/simple_meshes.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"

namespace mhe {
namespace utils {

bool create_plane(Mesh& mesh, const Context& context)
{
	StandartGeometryLayout::Vertex vertexes[4];
	vertexes[0].pos.set(-0.5f, -0.5f, 0.0);
	vertexes[1].pos.set(-0.5f, 0.5f, 0.0);
	vertexes[2].pos.set(0.5f, 0.5f, 0.0);
	vertexes[3].pos.set(0.5f, -0.5f, 0.0);

	vertexes[0].nrm.set(0.0f, 0.0f, 1.0f);
	vertexes[1].nrm.set(0.0f, 0.0f, 1.0f);
	vertexes[2].nrm.set(0.0f, 0.0f, 1.0f);
	vertexes[3].nrm.set(0.0f, 0.0f, 1.0f);

	vertexes[0].tex.set(0.0f, 0.0f);
	vertexes[1].tex.set(0.0f, 1.0f);
	vertexes[2].tex.set(1.0f, 1.0f);
	vertexes[3].tex.set(1.0f, 0.0f);

	uint32_t indexes[6] = {0, 1, 2, 2, 3, 0};

	mesh.vbuffer = context.vertex_buffer_pool.create();
	mesh.ibuffer = context.index_buffer_pool.create();
	mesh.state = context.render_state_pool.create();

	mesh.render_data.elements_number = 2;

	RenderStateDesc desc;
	RenderState& render_state = context.render_state_pool.get(mesh.state);
	if (!render_state.init(desc))
		return false;

	VertexBuffer& vbuffer = context.vertex_buffer_pool.get(mesh.vbuffer);
	if (!vbuffer.init(buffer_update_type_static,
					  reinterpret_cast<const uint8_t*>(&vertexes[0]), 4 * sizeof(StandartGeometryLayout::Vertex), sizeof(StandartGeometryLayout::Vertex)))
		return false;
	IndexBuffer& ibuffer = context.index_buffer_pool.get(mesh.ibuffer);
	return ibuffer.init(vbuffer, indexes, 6);
}

bool create_axes(Mesh& mesh, const Context& context)
{
	StandartGeometryLayout::Vertex vertexes[6];
	vertexes[0].pos.set(0.0f, 0.0f, 0.0f); 
	vertexes[1].pos.set(1.0f, 0.0f, 0.0f);
	vertexes[2].pos.set(0.0f, 0.0f, 0.0f);
	vertexes[3].pos.set(0.0f, 1.0f, 0.0);
	vertexes[4].pos.set(0.0f, 0.0f, 0.0f);
	vertexes[5].pos.set(0.0f, 0.0f, 1.0f);

	uint32_t indexes[6] = {0, 1, 2, 3, 4, 5};

	mesh.vbuffer = context.vertex_buffer_pool.create();
	mesh.ibuffer = context.index_buffer_pool.create();
	mesh.state = context.render_state_pool.create();

	mesh.render_data.elements_number = 2;
	mesh.render_data.primitive = lines;

	RenderStateDesc desc;
	desc.depth.enabled = false;
	RenderState& render_state = context.render_state_pool.get(mesh.state);
	if (!render_state.init(desc))
		return false;

	VertexBuffer& vbuffer = context.vertex_buffer_pool.get(mesh.vbuffer);
	if (!vbuffer.init(buffer_update_type_static,
					  reinterpret_cast<const uint8_t*>(&vertexes[0]), 6 * sizeof(StandartGeometryLayout::Vertex), sizeof(StandartGeometryLayout::Vertex)))
		return false;
	IndexBuffer& ibuffer = context.index_buffer_pool.get(mesh.ibuffer);
	return ibuffer.init(vbuffer, indexes, 6);
}

bool create_skybox_quad(Mesh& mesh, const Context& context)
{
	SkyboxLayout::Vertex vertexes[3];
	vertexes[0].pos.set(-1.0f, -3.0f, 1.0f, 1.0f);
	vertexes[1].pos.set(-1.0f, 1.0f, 1.0f, 1.0f);
	vertexes[2].pos.set(3.0f, 1.0f, 1.0f, 1.0f);

	uint32_t indexes[3] = {0, 1, 2};

	mesh.vbuffer = context.vertex_buffer_pool.create();
	mesh.ibuffer = context.index_buffer_pool.create();
	mesh.state = context.render_state_pool.create();

	mesh.render_data.elements_number = 1;

	RenderStateDesc desc;
	desc.depth.enabled = false;
	RenderState& render_state = context.render_state_pool.get(mesh.state);
	if (!render_state.init(desc))
		return false;

	VertexBuffer& vbuffer = context.vertex_buffer_pool.get(mesh.vbuffer);
	if (!vbuffer.init(buffer_update_type_static,
					  reinterpret_cast<const uint8_t*>(&vertexes[0]), 3 * sizeof(SkyboxLayout::Vertex), sizeof(SkyboxLayout::Vertex)))
		return false;
	IndexBuffer& ibuffer = context.index_buffer_pool.get(mesh.ibuffer);
	return ibuffer.init(vbuffer, indexes, 3);
}

}}
