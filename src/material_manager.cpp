#include "material_manager.hpp"

#include "context.hpp"

namespace mhe {

material_ptr MaterialManager::get_material(const std::string& texture_name,
										   const std::string& shader_name) const
{
	std::string name = texture_name + shader_name;
	resmap& res = resources();
	typename resmap::const_iterator it = res.find(name);
	if (it != res.end())
		return it->second;
	const Context* context = helper();
	assert(context != nullptr);
	texture_ptr texture = context->texture_manager().get(texture_name);
	shader_program_ptr shader_program = context->shader_manager().get(shader_name);
	material_ptr material(new Material(texture, shader_program));
	res[name] = material;
	return material;
}

}
