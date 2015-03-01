#include "res/mhe_binary_mesh.hpp"

#include "res/scene_export.hpp"
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

template <class Vertex>
bool init_mesh(Mesh& mesh, uint8_t layout, const std::vector<Vertex>& vertexes, const std::vector<uint32_t>& indexes, 
	const std::vector<MaterialExportData>& materials_data, const std::vector<MeshPartExportData>& parts_data, const Context* context)
{
	mesh.vbuffer = context->vertex_buffer_pool.create();
	mesh.ibuffer = context->index_buffer_pool.create();

	VertexBuffer& vbuffer = context->vertex_buffer_pool.get(mesh.vbuffer);
	if (!vbuffer.init(buffer_update_type_static,
		reinterpret_cast<const uint8_t*>(&vertexes[0]), vertexes.size() * sizeof(Vertex), sizeof(Vertex)))
	{
		ERROR_LOG("Vertex buffer initialization failed");
		return false;
	}
	IndexBuffer& ibuffer = context->index_buffer_pool.get(mesh.ibuffer);
	if (!ibuffer.init(vbuffer, &indexes[0], indexes.size()))
	{
		ERROR_LOG("Index buffer initialization failed");
		return false;
	}

	mesh.parts.reserve(parts_data.size());
	for (size_t i = 0; i < parts_data.size(); ++i)
	{
		const MeshPartExportData& part_data = parts_data[i];
		if (part_data.faces_number == 0 || part_data.material_index >= materials_data.size())
		{
			ERROR_LOG("Mesh contains invalid part:" << i);
			continue;
		}
		const MaterialExportData& material_data = materials_data[part_data.material_index];

		MeshPart part;
		part.render_data.primitive = triangle;
		part.render_data.vbuffer   = vbuffer.id();
		part.render_data.vbuffer_offset = part_data.vbuffer_offset;
		part.render_data.ibuffer   = ibuffer.id();
		part.render_data.ibuffer_offset = part_data.ibuffer_offset;
		part.render_data.elements_number = part_data.faces_number;
		part.render_data.indexes_number = part_data.faces_number * 3;
		part.render_data.layout = layout;

		part.material_data.layout = layout;
		part.material_data.albedo_texture = material_data.albedo_texture.name;
		part.material_data.normalmap_texture = material_data.normalmap_texture.name;

		mesh.parts.push_back(part);
	}

	return !mesh.parts.empty();
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

void load_texture(TextureExportData& texture_data, std::istream& stream)
{
	uint32_t len;
	stream.read(reinterpret_cast<char*>(&len), 4);
	if (len != 0)
	{
		stream.read(texture_data.name.buffer(), len);
		texture_data.name.resize(len);
	}
	stream.read(reinterpret_cast<char*>(&texture_data.mode), 1);
}

bool load_mesh_version2(Mesh& mesh, std::istream& stream, const Context* context)
{
	uint8_t layout;
	stream.read(reinterpret_cast<char*>(&layout), 1);
	uint32_t size, vertex_size;
	stream.read(reinterpret_cast<char*>(&vertex_size), sizeof(uint32_t));
	ASSERT(vertex_size == sizeof(StandartGeometryLayout::Vertex), "Invalid vertex size");
    stream.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
    // TODO: check layout
    std::vector<StandartGeometryLayout::Vertex> vertexes(size);
    stream.read(reinterpret_cast<char*>(&vertexes[0]), size * vertex_size);

    stream.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
    std::vector<uint32_t> indexes(size);
    stream.read(reinterpret_cast<char*>(&indexes[0]), size * 4);
	// materials
	uint32_t materials_number;
	stream.read(reinterpret_cast<char*>(&materials_number), 4);
	std::vector<MaterialExportData> material_data(materials_number);
	for (size_t i = 0; i < materials_number; ++i)
	{
		char type;
		stream.read(&type, 1);
		ASSERT(type == 'a', "Invalid material data");
		load_texture(material_data[i].albedo_texture, stream);
		stream.read(&type, 1);
		ASSERT(type == 'n', "Invalid material data");
		load_texture(material_data[i].normalmap_texture, stream);
	}
	// parts
	uint32_t parts_number;
	stream.read(reinterpret_cast<char*>(&parts_number), 4);
	std::vector<MeshPartExportData> parts_data(parts_number);
	stream.read(reinterpret_cast<char*>(&parts_data[0]), parts_number * sizeof(MeshPartExportData));

	return detail::init_mesh(mesh, layout, vertexes, indexes, material_data, parts_data, context);
}

}

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
	else if (version == 2)
		return detail::load_mesh_version2(mesh, stream, context);
    return false;
}

}
