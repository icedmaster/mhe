#ifndef __MESH_LOADER_HPP__
#define __MESH_LOADER_HPP__

#include "render/mesh.hpp"

namespace mhe {

struct Context;

struct MeshLoader
{
	typedef Mesh type;
    typedef MeshInstance instance_type;
	typedef Context context_type;
	static MHE_EXPORT bool load(type& res, const std::string& name, const context_type* context);
    static MHE_EXPORT type& get_resource(MeshInstance& instance)
    {
        return instance.mesh;
    }

    static MHE_EXPORT bool setup_instance(MeshInstance& instance, const context_type* context);
};

}

#endif
