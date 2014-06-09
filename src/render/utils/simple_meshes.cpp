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

	mesh.layout = StandartGeometryLayout::handle;
	mesh.vbuffer = context.vertex_buffer_pool.create();
	mesh.ibuffer = context.index_buffer_pool.create();

	VertexBuffer& vbuffer = context.vertex_buffer_pool.get(mesh.vbuffer);
	if (!vbuffer.init(buffer_update_type_static,
					  reinterpret_cast<const uint8_t*>(&vertexes[0]), 4 * sizeof(StandartGeometryLayout::Vertex), sizeof(StandartGeometryLayout::Vertex)))
		return false;
	IndexBuffer& ibuffer = context.index_buffer_pool.get(mesh.ibuffer);
	return ibuffer.init(indexes, 6);
}

}}
