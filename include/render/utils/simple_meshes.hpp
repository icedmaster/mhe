#ifndef __SIMPLE_MESHES_HPP__
#define __SIMPLE_MESHES_HPP__

#include "core/compiler.hpp"

namespace mhe {

struct MeshInstance;
struct NodeInstance;
struct Context;

namespace utils {

bool MHE_EXPORT create_plane(MeshInstance& mesh, const Context& context);
bool MHE_EXPORT create_axes(MeshInstance& mesh, const Context& context);
bool MHE_EXPORT create_skybox_quad(MeshInstance& mesh, const Context& context);
bool MHE_EXPORT create_fullscreen_quad(MeshInstance& mesh, const Context& context);
bool MHE_EXPORT create_sphere(MeshInstance& mesh_instance, const Context& context, int subdivision);
bool MHE_EXPORT create_conus(MeshInstance& mesh_instance, const Context& context, float radius, float height, int subdivision);

bool MHE_EXPORT create_plane(NodeInstance& node, const Context& context);
bool MHE_EXPORT create_sphere(NodeInstance& node, const Context& context, int subdivision);
bool MHE_EXPORT create_conus(NodeInstance& node, const Context& context, float radius, float height, int subdivision);

}}

#endif
