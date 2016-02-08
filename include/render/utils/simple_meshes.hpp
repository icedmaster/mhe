#ifndef __SIMPLE_MESHES_HPP__
#define __SIMPLE_MESHES_HPP__

#include "core/compiler.hpp"
#include "core/types.hpp"

namespace mhe {

struct MeshInstance;
struct NodeInstance;
struct Context;

namespace utils {

enum
{
    mesh_creation_flag_none = 0,
    mesh_creation_flag_trace_data = 1
};

bool MHE_EXPORT create_plane(MeshInstance& mesh, const Context& context, uint32_t flags = mesh_creation_flag_none);
bool MHE_EXPORT create_axes(MeshInstance& mesh, const Context& context);
bool MHE_EXPORT create_skybox_quad(MeshInstance& mesh, const Context& context);
bool MHE_EXPORT create_fullscreen_quad(MeshInstance& mesh, const Context& context);
bool MHE_EXPORT create_sphere(MeshInstance& mesh_instance, const Context& context, int subdivision, uint32_t flags = mesh_creation_flag_none);
bool MHE_EXPORT create_conus(MeshInstance& mesh_instance, const Context& context, float radius, float height, int subdivision);
bool MHE_EXPORT create_cube(MeshInstance& mesh_instance, const Context& context, uint32_t flags = mesh_creation_flag_none);

bool MHE_EXPORT create_plane(NodeInstance& node, const Context& context, uint32_t flags = mesh_creation_flag_none);
bool MHE_EXPORT create_sphere(NodeInstance& node, const Context& context, int subdivision, uint32_t flags = mesh_creation_flag_none);
bool MHE_EXPORT create_conus(NodeInstance& node, const Context& context, float radius, float height, int subdivision);
bool MHE_EXPORT create_cube(NodeInstance& node, const Context& context, uint32_t flags = mesh_creation_flag_none);

}}

#endif
