#include "res/mesh_loader.hpp"

#include "res/mhe_binary_mesh.hpp"

#include <fstream>

namespace mhe {

bool MeshLoader::load(type& res, const std::string& name, const context_type* context)
{
	std::ifstream f(name.c_str(), std::ios::in | std::ios::binary);
	if (!f.is_open())
		return false;
	bool result = mhe_binary_mesh_load(res, f, context);
	f.close();
	return result;
}

bool MeshLoader::setup_instance(MeshInstance& instance)
{
    instance.instance_parts.resize(instance.mesh.parts.size());
    return true;
}

}
