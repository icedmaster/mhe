#include "render/mesh.hpp"

namespace mhe {

bool create_mesh(Mesh& mesh, const uint8_t* vertices, size_t vertices_size, const uint32_t* indices, size_t indices_size,
    size_t parts_number, Context& context)
{
    return false;
}

void destroy_mesh(Mesh& mesh, Context& context)
{

}

bool create_mesh_instance(MeshInstance& mesh, const uint8_t* vertices, size_t vertices_size, const uint32_t* indices, size_t indices_size,
    size_t parts_number, Context& context)
{
    return false;
}

void destroy_mesh_instance(MeshInstance& mesh, Context& context)
{

}

}
