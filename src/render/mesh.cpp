#include "render/mesh.hpp"

namespace mhe {

bool create_mesh(Mesh& mesh, const uint8_t* vertices, size_t vertices_size, const uint32_t* indices, size_t indices_size,
    size_t parts_number, Context& context)
{
    UNUSED(mesh);
    UNUSED(vertices);
    UNUSED(vertices_size);
    UNUSED(indices);
    UNUSED(indices_size);
    UNUSED(parts_number);
    UNUSED(context);
    return false;
}

void destroy_mesh(Mesh& mesh, Context& context)
{
    UNUSED(mesh);
    UNUSED(context);
}

bool create_mesh_instance(MeshInstance& mesh, const uint8_t* vertices, size_t vertices_size, const uint32_t* indices, size_t indices_size,
    size_t parts_number, Context& context)
{
    UNUSED(mesh);
    UNUSED(vertices);
    UNUSED(vertices_size);
    UNUSED(indices);
    UNUSED(indices_size);
    UNUSED(parts_number);
    UNUSED(context);
    return false;
}

void destroy_mesh_instance(MeshInstance& mesh, Context& context)
{
    UNUSED(mesh);
    UNUSED(context);
}

}
