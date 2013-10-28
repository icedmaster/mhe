#ifndef __MATERIAL_MANAGER_HPP__
#define __MATERIAL_MANAGER_HPP__

#include "material_loader.hpp"
#include "resource_manager.hpp"

namespace mhe {

class MaterialManager : public ResourceManager<MaterialLoader>
{
public:
	material_ptr get_material(const std::string& texture_name, const std::string& shader_name) const;
};

}

#endif
