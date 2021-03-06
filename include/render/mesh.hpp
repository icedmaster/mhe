#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "math/matrix.hpp"
#include "math/aabb.hpp"
#include "render_buffer.hpp"
#include "render_state.hpp"
#include "material.hpp"
#include "render_target.hpp"
#include "render_common.hpp"

namespace mhe {

struct RenderData
{
    size_t elements_number; // the number of faces
    uint32_t vbuffer_offset;
    uint32_t ibuffer_offset;
    size_t indexes_number;  // the total size of index buffer is used if indexes_number is zero
    Primitive primitive;
    Layout::IdType layout;
    VertexBuffer::IdType vbuffer;
    IndexBuffer::IdType ibuffer;

    RenderData() :
        vbuffer_offset(0), ibuffer_offset(0), indexes_number(0), primitive(triangle),
        layout(Layout::invalid_id), vbuffer(VertexBuffer::invalid_id), ibuffer(IndexBuffer::invalid_id)
    {}
};

struct Bone
{
    static const uint32_t invalid_id = static_cast<uint32_t>(-1);
    string name;
    uint32_t id;
    uint32_t parent_id;
    mat4x4 inv_transform;   // inverse bind pose transform
    mat4x4 local_transform;
};

struct Skeleton
{
    mat4x4 root_transform;
    std::vector<Bone> bones;
};

struct SkeletonInstance
{
    TextureBufferHandleType texture_buffer;

    SkeletonInstance() : texture_buffer(InvalidHandle<TextureBufferHandleType>::id) {}
};

struct GIDataInstance
{
    TextureBufferHandleType texture_buffer;

    GIDataInstance() : texture_buffer(InvalidHandle<TextureBufferHandleType>::id) {}
};

struct MeshPart
{
    RenderData render_data;
    AABBf aabb;
    MaterialDataIdType material_id;
};

struct Mesh
{
    std::vector<MeshPart> parts;
    VertexBuffer::IdType vbuffer;
    IndexBuffer::IdType ibuffer;
    MeshTraceDataHandleType trace_data_id;
    MeshRawDataHandleType raw_data_id;
    AABBf aabb;
    Skeleton skeleton;

    Mesh() : vbuffer(VertexBuffer::invalid_id), ibuffer(IndexBuffer::invalid_id),
             trace_data_id(InvalidHandle<MeshTraceDataHandleType>::id), raw_data_id(InvalidHandle<MeshRawDataHandleType>::id)
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
    enum
    {
        cast_shadow = 1 << 0,
        receive_shadow = 1 << 1,
        cast_reflection = 1 << 2
    };

    MaterialInstance material;
    MaterialDataIdType material_id;
    RenderStateHandleType render_state_id;
    AABBInstanceHandleType aabb_id;
    uint16_t flags;
    bool visible : 1;

    MeshPartInstance() : material_id(InvalidHandle<MaterialDataIdType>::id),
        render_state_id(InvalidHandle<RenderStateHandleType>::id),
        aabb_id(InvalidHandle<AABBInstanceHandleType>::id),
        flags(cast_shadow | receive_shadow | cast_reflection),
        visible(true)
    {}
};

struct MeshInstance
{
    Mesh mesh;
    std::vector<MeshPartInstance> instance_parts;
    UniformBuffer::IdType shared_uniform;
    SkeletonInstance skeleton_instance;
    GIDataInstance gi_data;

    MeshInstance() : shared_uniform(UniformBuffer::invalid_id) {}
};

// TODO:
bool create_mesh(Mesh& mesh, const uint8_t* vertices, size_t vertices_size, const uint32_t* indices, size_t indices_size,
    size_t parts_number, Context& context);
void destroy_mesh(Mesh& mesh, Context& context);

bool create_mesh_instance(MeshInstance& mesh, const uint8_t* vertices, size_t vertices_size, const uint32_t* indices, size_t indices_size,
    size_t parts_number, Context& context);
void destroy_mesh_instance(MeshInstance& mesh, Context& context);

inline MeshPartInstance& add_part(MeshInstance& mesh)
{
    add_part(mesh.mesh);
    mesh.instance_parts.resize(mesh.instance_parts.size() + 1);
    return mesh.instance_parts.back();
}

}

#endif
