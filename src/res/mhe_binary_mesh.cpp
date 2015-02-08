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

    mesh.parts.resize(1);
    mesh.parts[0].render_data.elements_number = indexes.size() / 3;
    mesh.parts[0].render_data.vbuffer = mesh.vbuffer;
    mesh.parts[0].render_data.ibuffer = mesh.ibuffer;

	VertexBuffer& vbuffer = context->vertex_buffer_pool.get(mesh.vbuffer);
	if (!vbuffer.init(buffer_update_type_static,
		reinterpret_cast<const uint8_t*>(&vertexes[0]), vertexes.size() * sizeof(Vertex), sizeof(Vertex)))
		return false;
	IndexBuffer& ibuffer = context->index_buffer_pool.get(mesh.ibuffer);
	return ibuffer.init(vbuffer, &indexes[0], indexes.size());
}

bool load_mesh_version1(Mesh& mesh, std::istream& stream, const Context* context)
{
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

const uint8_t supported_version = 1;

bool mhe_binary_mesh_load(Mesh& mesh, std::istream& stream, const Context* context)
{
    char header[3];
	stream.read(header, 3);
	if (memcmp(header, "mhe", 3))
		return false;

    char version;
	stream.read(&version, 1);
    if (version == 1)
        return detail::load_mesh_version1(mesh, stream, context);
    return false;
}

}
