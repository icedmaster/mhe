#include "res/mhe_binary_mesh.hpp"

#include "render/context.hpp"
#include "render/layouts.hpp"

namespace mhe {

namespace detail {

template <class Vertex>
bool init_mesh(Mesh& mesh, const std::vector<Vertex>& vertexes, const std::vector<uint32_t>& indexes, const Context* context)
{
	mesh.vbuffer = context->vertex_buffer_pool.create();
	mesh.ibuffer = context->index_buffer_pool.create();
	mesh.state = context->render_state_pool.create();

	mesh.render_data.elements_number = indexes.size() / 3;

	RenderStateDesc desc;
	RenderState& render_state = context->render_state_pool.get(mesh.state);
	if (!render_state.init(desc))
		return false;

	VertexBuffer& vbuffer = context->vertex_buffer_pool.get(mesh.vbuffer);
	if (!vbuffer.init(buffer_update_type_static,
		reinterpret_cast<const uint8_t*>(&vertexes[0]), vertexes.size() * sizeof(Vertex), sizeof(Vertex)))
		return false;
	IndexBuffer& ibuffer = context->index_buffer_pool.get(mesh.ibuffer);
	return ibuffer.init(vbuffer, &indexes[0], indexes.size());
}

}

const char supported_version = 1;

bool mhe_binary_mesh_load(Mesh& mesh, std::istream& stream, const Context* context)
{
	char header[3];
	stream.read(header, 3);
	if (memcmp(header, "mhe", 3))
		return false;

	char version;
	stream.read(&version, 1);
	if (version != supported_version)
		return false;

	uint32_t size, vertex_size;
	stream.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
	stream.read(reinterpret_cast<char*>(&vertex_size), sizeof(uint32_t));
	ASSERT(vertex_size == sizeof(StandartGeometryLayout::Vertex), "Invalid vertex size");
	// TODO: check layout
	std::vector<StandartGeometryLayout::Vertex> vertexes(size);
	stream.read(reinterpret_cast<char*>(&vertexes[0]), size * vertex_size);

	uint32_t index_size;
	stream.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
	stream.read(reinterpret_cast<char*>(&index_size), sizeof(uint32_t));
	std::vector<uint32_t> indexes(size);
	stream.read(reinterpret_cast<char*>(&indexes[0]), size * index_size);

	return detail::init_mesh(mesh, vertexes, indexes, context);
}

}
