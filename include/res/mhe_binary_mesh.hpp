#ifndef __MHE_BINARY_MESH_HPP__
#define __MHE_BINARY_MESH_HPP__

#include <fstream>

namespace mhe {

struct Mesh;
struct Context;

bool mhe_binary_mesh_load(Mesh& mesh, std::istream& stream, const Context* context);

}

#endif
