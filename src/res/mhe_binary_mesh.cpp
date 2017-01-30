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
void init_trace_data(Mesh& mesh, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, Context* context)
{
    mesh.trace_data_id = context->mesh_trace_data_pool.create();
    MeshGrid& grid = context->mesh_trace_data_pool.get(mesh.trace_data_id).grid;
    create_grid(grid, vertices, vertices.size(), indices, indices.size());
}

template <class Vertex>
bool init_mesh(Mesh& mesh, uint8_t layout, 
    const std::vector<Vertex>& vertexes, const std::vector<uint32_t>& indexes, 
    const std::vector<MeshPartExportData>& parts_data, Context* context, const res::Mesh& dbmesh)
{
    init_trace_data(mesh, vertexes, indexes, context);

    mesh.aabb = dbmesh.aabb;

    mesh.vbuffer = context->vertex_buffer_pool.create();
    mesh.ibuffer = context->index_buffer_pool.create();

    pool_initializer<VertexBufferPool> vbuffer_wr(context->vertex_buffer_pool);
    pool_initializer<IndexBufferPool> ibuffer_wr(context->index_buffer_pool);

    VertexBuffer& vbuffer = vbuffer_wr.object();
    if (!vbuffer.init(buffer_update_type_static,
        reinterpret_cast<const uint8_t*>(&vertexes[0]), vertexes.size() * sizeof(Vertex), sizeof(Vertex)))
    {
        ERROR_LOG("Vertex buffer initialization failed");
        return false;
    }
    IndexBuffer& ibuffer = ibuffer_wr.object();
    if (!ibuffer.init(vbuffer, &indexes[0], indexes.size()))
    {
        ERROR_LOG("Index buffer initialization failed");
        return false;
    }

    mesh.vbuffer = vbuffer_wr.take();
    mesh.ibuffer = ibuffer_wr.take();

    mesh.parts.reserve(parts_data.size());
    for (size_t i = 0; i < parts_data.size(); ++i)
    {
        const MeshPartExportData& part_data = parts_data[i];
        if (part_data.faces_number == 0)
        {
            ERROR_LOG("Mesh contains invalid part:" << i);
            continue;
        }

        MeshPart part;
        part.render_data.primitive = triangle;
        part.render_data.vbuffer = vbuffer.id();
        part.render_data.vbuffer_offset = part_data.vbuffer_offset;
        part.render_data.ibuffer = ibuffer.id();
        part.render_data.ibuffer_offset = part_data.ibuffer_offset;
        part.render_data.elements_number = part_data.faces_number;
        part.render_data.indexes_number = part_data.faces_number * 3;
        part.render_data.layout = layout;

        context->material_manager.get(part.material_id, dbmesh.parts[i].material);

        part.aabb = dbmesh.parts[i].aabb;

        mesh.parts.push_back(part);
    }

    INFO_LOG("Loaded a mesh:" << dbmesh.binary << " AABB:" << dbmesh.aabb);

    return !mesh.parts.empty();
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

void load_bone(Bone& bone, std::istream& stream)
{
    load_string(bone.name, stream);
    stream.read(reinterpret_cast<char*>(&bone.id), 4);
    stream.read(reinterpret_cast<char*>(&bone.parent_id), 4);
    stream.read(reinterpret_cast<char*>(&bone.inv_transform), sizeof(mat4x4));
    stream.read(reinterpret_cast<char*>(&bone.local_transform), sizeof(mat4x4));
}

bool load_mesh_desc(mhe::res::Mesh& res_mesh, const char* filename)
{
    mhe::res::XMLDeserializer deserializer(filename, "mesh");
    return res_mesh.read(deserializer);
}

bool load_mesh_version3(Mesh& mesh, const FilePath& filename, Context* context)
{
    res::Mesh res_mesh;
    if (!load_mesh_desc(res_mesh, filename.c_str()))
    {
        ERROR_LOG("Couldn't load a description for a mesh:" << filename);
        return false;
    }

    const FilePath& path = utils::get_file_path(mhe::FilePath(filename));
    FilePath binary_filename = utils::path_join(path, res_mesh.binary);
    std::ifstream stream(binary_filename.c_str(), std::ios::binary);
    if (!stream.is_open())
    {
        ERROR_LOG("Couldn't open a binary mesh file:" << binary_filename);
        return false;
    }

    uint8_t header[3];
    stream.read(reinterpret_cast<char*>(header), 3);
    ASSERT(::memcmp(header, mhe_header, 3) == 0, "Invalid binary file header");
    
    uint8_t version = 0;
    stream.read(reinterpret_cast<char*>(&version), 1);
    ASSERT(version == 3, "Invalid binary file version");

    uint8_t layout;
    stream.read(reinterpret_cast<char*>(&layout), 1);
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
        stream.close();
        return false;
    }

    stream.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
    std::vector<uint32_t> indexes(size);
    stream.read(reinterpret_cast<char*>(&indexes[0]), size * 4);
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
        return detail::init_mesh(mesh, layout, skinned_vertices, indexes, parts_data, context, res_mesh);
    }
    else
        return detail::init_mesh(mesh, layout, vertices, indexes, parts_data, context, res_mesh);
}

}

bool mhe_binary_mesh_load(Mesh& mesh, const FilePath& filename, Context* context)
{
    return detail::load_mesh_version3(mesh, filename, context);
}

}
