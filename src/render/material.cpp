#include "render/material.hpp"
#include "render/context.hpp"

namespace mhe {

MaterialId MaterialManager::get(const MaterialInitializationData& data) const
{
	if (!data.name.empty())
	{
		for (MaterialsMap::iterator it = materials_.begin(); it != materials_.end(); ++it)
		{
			if (it->value.name == data.name)
				return it->key;
		}
	}

	MaterialId id = materials_.size();
	MaterialData& material_data = materials_[id];
	material_data.name = data.name;
	material_data.lighting_model = data.lighting_model;
	material_data.render_data = data.render_data;
	for (size_t i = 0; i < material_textures_number; ++i)
	{
		if (data.textures[i].empty()) continue;
		context_->texture_manager.get(material_data.textures[i], data.textures[i]);
	}
	return id;
}

}
