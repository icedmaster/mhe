#ifndef __SIMPLE_MESHES_HPP__
#define __SIMPLE_MESHES_HPP__

#include "core/compiler.hpp"

namespace mhe {

struct Mesh;
struct Context;

namespace utils {

bool MHE_EXPORT create_plane(Mesh& mesh, const Context& context);

}}

#endif
