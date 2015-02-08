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

bool MHE_EXPORT create_plane(NodeInstance& node, const Context& context);

}}

#endif
