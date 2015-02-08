#include "render/utils/simple_meshes.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/node.hpp"
#include "render/instances.hpp"

namespace mhe {
namespace utils {

bool create_plane(MeshInstance& mesh_instance, const Context& context)
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

    Mesh& mesh = mesh_instance.mesh;

    mesh.parts[0].render_data.vbuffer = context.vertex_buffer_pool.create();
    mesh.parts[0].render_data.ibuffer = context.index_buffer_pool.create();

    mesh.parts[0].render_data.elements_number = 2;

    VertexBuffer& vbuffer = context.vertex_buffer_pool.get(mesh.parts[0].render_data.vbuffer);
	if (!vbuffer.init(buffer_update_type_static,
					  reinterpret_cast<const uint8_t*>(&vertexes[0]), 4 * sizeof(StandartGeometryLayout::Vertex), sizeof(StandartGeometryLayout::Vertex)))
		return false;
    IndexBuffer& ibuffer = context.index_buffer_pool.get(mesh.parts[0].render_data.ibuffer);
	return ibuffer.init(vbuffer, indexes, 6);
}

bool create_axes(MeshInstance& mesh_instance, const Context& context)
{
	StandartGeometryLayout::Vertex vertexes[6];
	vertexes[0].pos.set(0.0f, 0.0f, 0.0f); 
	vertexes[1].pos.set(1.0f, 0.0f, 0.0f);
	vertexes[2].pos.set(0.0f, 0.0f, 0.0f);
	vertexes[3].pos.set(0.0f, 1.0f, 0.0);
	vertexes[4].pos.set(0.0f, 0.0f, 0.0f);
	vertexes[5].pos.set(0.0f, 0.0f, 1.0f);

	uint32_t indexes[6] = {0, 1, 2, 3, 4, 5};

    Mesh& mesh = mesh_instance.mesh;

    mesh.parts[0].render_data.vbuffer = context.vertex_buffer_pool.create();
    mesh.parts[0].render_data.ibuffer = context.index_buffer_pool.create();

    mesh.parts[0].render_data.elements_number = 2;
    mesh.parts[0].render_data.primitive = lines;

    VertexBuffer& vbuffer = context.vertex_buffer_pool.get(mesh.parts[0].render_data.vbuffer);
	if (!vbuffer.init(buffer_update_type_static,
					  reinterpret_cast<const uint8_t*>(&vertexes[0]), 6 * sizeof(StandartGeometryLayout::Vertex), sizeof(StandartGeometryLayout::Vertex)))
		return false;
    IndexBuffer& ibuffer = context.index_buffer_pool.get(mesh.parts[0].render_data.ibuffer);
	return ibuffer.init(vbuffer, indexes, 6);
}

bool create_skybox_quad(MeshInstance& mesh, const Context& context)
{
	SkyboxLayout::Vertex vertexes[3];
	vertexes[0].pos.set(-1.0f, -3.0f, 1.0f, 1.0f);
	vertexes[1].pos.set(-1.0f, 1.0f, 1.0f, 1.0f);
	vertexes[2].pos.set(3.0f, 1.0f, 1.0f, 1.0f);

	uint32_t indexes[3] = {0, 1, 2};

    MeshPart& part = mesh.mesh.parts[0];

    part.render_data.vbuffer = context.vertex_buffer_pool.create();
    part.render_data.ibuffer = context.index_buffer_pool.create();

    part.render_data.elements_number = 1;

    VertexBuffer& vbuffer = context.vertex_buffer_pool.get(part.render_data.vbuffer);
	if (!vbuffer.init(buffer_update_type_static,
					  reinterpret_cast<const uint8_t*>(&vertexes[0]), 3 * sizeof(SkyboxLayout::Vertex), sizeof(SkyboxLayout::Vertex)))
		return false;
    IndexBuffer& ibuffer = context.index_buffer_pool.get(part.render_data.ibuffer);
	return ibuffer.init(vbuffer, indexes, 3);
}

bool create_fullscreen_quad(MeshInstance& mesh, const Context& context)
{
	FullscreenLayout::Vertex vertexes[3];
	vertexes[0].pos.set(-1.0f, -3.0f, 1.0f, 1.0f);
	vertexes[1].pos.set(-1.0f, 1.0f, 1.0f, 1.0f);
	vertexes[2].pos.set(3.0f, 1.0f, 1.0f, 1.0f);
	vertexes[0].tex.set(0.0f, -1.0f);
	vertexes[1].tex.set(0.0f, 1.0f);
	vertexes[2].tex.set(2.0f, 1.0f);

	uint32_t indexes[3] = {0, 1, 2};

    MeshPart& part = mesh.mesh.parts[0];

    part.render_data.vbuffer = context.vertex_buffer_pool.create();
    part.render_data.ibuffer = context.index_buffer_pool.create();

    part.render_data.elements_number = 1;

    VertexBuffer& vbuffer = context.vertex_buffer_pool.get(part.render_data.vbuffer);
	if (!vbuffer.init(buffer_update_type_static,
					  reinterpret_cast<const uint8_t*>(&vertexes[0]), 3 * sizeof(FullscreenLayout::Vertex), sizeof(FullscreenLayout::Vertex)))
		return false;
    IndexBuffer& ibuffer = context.index_buffer_pool.get(part.render_data.ibuffer);
	return ibuffer.init(vbuffer, indexes, 3);
}

bool create_plane(NodeInstance& node, const Context& context)
{
    add_part(node.mesh);
    return create_plane(node.mesh, context);
}
}}
