#include "res/mhe_binary_mesh.hpp"

#include "res/scene_export.hpp"
#include "render/context.hpp"
#include "render/layouts.hpp"
#include "render/material.hpp"
#include "render/uniforms.hpp"
#include "render/mesh_grid.hpp"

namespace mhe {

namespace detail {

template <class Vertex>
void init_trace_data(Mesh& mesh, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const Context* context)
{
	mesh.trace_data_id = context->mesh_trace_data_pool.create();
	MeshGrid& grid = context->mesh_trace_data_pool.get(mesh.trace_data_id).grid;
	create_grid(grid, vertices, vertices.size(), indices, indices.size());
}

template <class Vertex>
bool init_mesh(Mesh& mesh, const std::vector<Vertex>& vertexes, const std::vector<uint32_t>& indexes, const Context* context)
{
	init_trace_data(mesh, vertexes, indexes, context);

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
bool init_mesh(Mesh& mesh, uint8_t layout, const MeshExportData& mesh_export_data, 
	const std::vector<Vertex>& vertexes, const std::vector<uint32_t>& indexes, 
	const std::vector<MaterialExportData>& materials_data, const std::vector<MeshPartExportData>& parts_data, const Context* context)
{
	init_trace_data(mesh, vertexes, indexes, context);

	mesh.aabb = mesh_export_data.aabb;

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

		MaterialInitializationData initialization_data;
		initialization_data.name = material_data.name;
		initialization_data.lighting_model = material_data.lighting_model;
		initialization_data.render_data = material_data.data;
		initialization_data.render_data.glossiness = default_glossiness;
		initialization_data.textures[albedo_texture_unit] = material_data.albedo_texture.name;
		initialization_data.textures[normal_texture_unit] = material_data.normalmap_texture.name;
		part.material_id = context->material_manager.get(initialization_data);

		part.aabb = part_data.aabb;

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

template <class Str>
void load_string(Str& str, std::istream& stream)
{
	uint32_t len;
	stream.read(reinterpret_cast<char*>(&len), 4);
	if (len != 0)
	{
		stream.read(str.buffer(), len);
		str.resize(len);
	}
}

void load_texture(TextureExportData& texture_data, std::istream& stream)
{
	load_string(texture_data.name, stream);
	stream.read(reinterpret_cast<char*>(&texture_data.mode), 1);
}

void load_bone(Bone& bone, std::istream& stream)
{
	load_string(bone.name, stream);
	stream.read(reinterpret_cast<char*>(&bone.id), 4);
	stream.read(reinterpret_cast<char*>(&bone.parent_id), 4);
	stream.read(reinterpret_cast<char*>(&bone.inv_transform), sizeof(mat4x4));
	stream.read(reinterpret_cast<char*>(&bone.local_transform), sizeof(mat4x4));
}

bool load_mesh_version2(Mesh& mesh, std::istream& stream, const Context* context)
{
	uint8_t layout;
	stream.read(reinterpret_cast<char*>(&layout), 1);
	MeshExportData mesh_export_data;
	stream.read(reinterpret_cast<char*>(&mesh_export_data), sizeof(MeshExportData));
	uint32_t size, vertex_size;
	stream.read(reinterpret_cast<char*>(&vertex_size), sizeof(uint32_t));
    stream.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));

    std::vector<StandartGeometryLayout::Vertex> vertices;
	std::vector<SkinnedGeometryLayout::Vertex> skinned_vertices;

	if (layout == StandartGeometryLayout::handle)
	{
		ASSERT(vertex_size == sizeof(StandartGeometryLayout::Vertex), "Invalid vertex size");
		vertices.resize(size);
		stream.read(reinterpret_cast<char*>(&vertices[0]), size * vertex_size);
	}
	else if (layout == SkinnedGeometryLayout::handle)
	{
		ASSERT(vertex_size == sizeof(SkinnedGeometryLayout::Vertex), "Invalid vertex size");
		skinned_vertices.resize(size);
		stream.read(reinterpret_cast<char*>(&skinned_vertices[0]), size * vertex_size);
	}
	else
	{
		ASSERT(0, "Unsupported for export layout found:" << layout);
		return false;
	}

    stream.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
    std::vector<uint32_t> indexes(size);
    stream.read(reinterpret_cast<char*>(&indexes[0]), size * 4);
	// materials
	uint32_t materials_number;
	stream.read(reinterpret_cast<char*>(&materials_number), 4);
	std::vector<MaterialExportData> material_data(materials_number);
	for (size_t i = 0; i < materials_number; ++i)
	{
		load_string(material_data[i].name, stream);
		load_string(material_data[i].material_system, stream);
		load_string(material_data[i].lighting_model, stream);
		stream.read(reinterpret_cast<char*>(&material_data[i].data), sizeof(MaterialRenderData));
		load_texture(material_data[i].albedo_texture, stream);
		load_texture(material_data[i].normalmap_texture, stream);
	}
	// parts
	uint32_t parts_number;
	stream.read(reinterpret_cast<char*>(&parts_number), 4);
	std::vector<MeshPartExportData> parts_data(parts_number);
	stream.read(reinterpret_cast<char*>(&parts_data[0]), parts_number * sizeof(MeshPartExportData));

	// skeleton if needed
	if (layout == SkinnedGeometryLayout::handle)
	{
		stream.read(reinterpret_cast<char*>(&mesh.skeleton.root_transform), sizeof(mat4x4));
		uint32_t bones_number;
		stream.read(reinterpret_cast<char*>(&bones_number), 4);
		mesh.skeleton.bones.resize(bones_number);
		for (size_t i = 0; i < bones_number; ++i)
		{
			load_bone(mesh.skeleton.bones[i], stream);
		}
		return detail::init_mesh(mesh, layout, mesh_export_data, skinned_vertices, indexes, material_data, parts_data, context);
	}
	else
		return detail::init_mesh(mesh, layout, mesh_export_data, vertices, indexes, material_data, parts_data, context);
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
