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

template <class V>
void subdivide(std::vector<V>& vertices, std::vector<uint32_t>& indices, const vec3& p1, const vec3& p2, const vec3& p3, int level)
{
    if (level > 0)
    {
        vec3 m1 = p1 + p2;
        vec3 m2 = p2 + p3;
        vec3 m3 = p3 + p1;
        m1.normalize();
        m2.normalize();
        m3.normalize();
        subdivide(vertices, indices, p1, m3, m1, level - 1);
        subdivide(vertices, indices, m1, m2, p2, level - 1);
        subdivide(vertices, indices, m2, m1, m3, level - 1);
        subdivide(vertices, indices, m3, p3, m2, level - 1);
    }
    else
    {
        V v;
        v.pos = p1;
        vertices.push_back(v);
        v.pos = p2;
        vertices.push_back(v);
        v.pos = p3;
        vertices.push_back(v);

        uint32_t start = indices.size();
        indices.push_back(start);
        indices.push_back(start + 1);
        indices.push_back(start + 2);
    }
}

bool create_sphere(MeshInstance& mesh_instance, const Context& context, int subdivision)
{
    std::vector<DebugLayout::Vertex> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(8 * 3 * (1 << (2 * subdivision)));
    indices.reserve(8 * 3 * (1 << (2 * subdivision)));

    vec3 l(-1, 0, 0);
    vec3 r(1, 0, 0);
    vec3 b(0, 0, -1);
    vec3 f(0, 0, 1);
    vec3 d(0, -1, 0);
    vec3 u(0, 1, 0);

    subdivide(vertices, indices, b, l, u, subdivision);
    subdivide(vertices, indices, r, b, u, subdivision);
    subdivide(vertices, indices, f, r, u, subdivision);
    subdivide(vertices, indices, l, f, u, subdivision);
    subdivide(vertices, indices, l, b, d, subdivision);
    subdivide(vertices, indices, b, r, d, subdivision);
    subdivide(vertices, indices, r, f, d, subdivision);
    subdivide(vertices, indices, f, l, d, subdivision);

    MeshPart& part = mesh_instance.mesh.parts[0];

    part.render_data.vbuffer = context.vertex_buffer_pool.create();
    part.render_data.ibuffer = context.index_buffer_pool.create();
    part.render_data.layout = DebugLayout::handle;

    VertexBuffer& vbuffer = context.vertex_buffer_pool.get(part.render_data.vbuffer);
    if (!vbuffer.init(buffer_update_type_static,
                      reinterpret_cast<const uint8_t*>(&vertices[0]), vertices.size() * sizeof(DebugLayout::Vertex), sizeof(DebugLayout::Vertex)))
        return false;
    IndexBuffer& ibuffer = context.index_buffer_pool.get(part.render_data.ibuffer);
    return ibuffer.init(vbuffer, &indices[0], indices.size());
}

bool create_conus(MeshInstance& mesh_instance, const Context& context, float radius, float height, int subdivision)
{
    std::vector<DebugLayout::Vertex> vertices(2 + subdivision);
    std::vector<uint32_t> indices(2 * 3 * subdivision);

    vec3 center = vec3::zero();
    vec3 top = vec3(0.0f, height, 0.0f);

    vertices[0].pos = center;
    vertices[1].pos = top;

    float angle = 2 * pi / subdivision;
    for (int i = 0; i < subdivision; ++i)
        vertices[i + 2].pos = vec3(radius * cos(i * angle), 0.0f, radius * sin(i * angle));
    // bottom
    for (int i = 0; i < subdivision; ++i)
    {
        indices[3 * i + 0] = i == subdivision - 1 ? 2 : i + 3;
        indices[3 * i + 1] = 0;
        indices[3 * i + 2] = i + 2;
    }
    // top
    for (int i = 0; i < subdivision; ++i)
    {
        indices[2 * 3 * i + 0] = i == subdivision - 1 ? 2 : i + 3;
        indices[2 * 3 * i + 1] = 1;
        indices[2 * 3 * i + 2] = i + 2;
    }

    MeshPart& part = mesh_instance.mesh.parts[0];

    part.render_data.vbuffer = context.vertex_buffer_pool.create();
    part.render_data.ibuffer = context.index_buffer_pool.create();
    part.render_data.layout = DebugLayout::handle;

    VertexBuffer& vbuffer = context.vertex_buffer_pool.get(part.render_data.vbuffer);
    if (!vbuffer.init(buffer_update_type_static,
                      reinterpret_cast<const uint8_t*>(&vertices[0]), vertices.size() * sizeof(DebugLayout::Vertex), sizeof(DebugLayout::Vertex)))
        return false;
    IndexBuffer& ibuffer = context.index_buffer_pool.get(part.render_data.ibuffer);
    return ibuffer.init(vbuffer, &indices[0], indices.size());
}

bool create_plane(NodeInstance& node, const Context& context)
{
    add_part(node.mesh);
    return create_plane(node.mesh, context);
}

bool create_sphere(NodeInstance& node, const Context& context, int subdivision)
{
    add_part(node.mesh);
    return create_sphere(node.mesh, context, subdivision);
}

bool create_conus(NodeInstance& node, const Context& context, float radius, float height, int subdivision)
{
    add_part(node.mesh);
    return create_conus(node.mesh, context, radius, height, subdivision);
}
}}
