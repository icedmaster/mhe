#ifndef __MESH_LOADER_HPP__
#define __MESH_LOADER_HPP__

#include "render/mesh.hpp"

namespace mhe {

struct Context;

struct MeshLoader
{
	typedef Mesh type;
	typedef Context context_type;
	static bool load(type& res, const std::string& name, const context_type* context);
};

}

#endif
