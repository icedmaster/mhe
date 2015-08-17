#ifndef __INITIALIZATION_HPP__
#define __INITIALIZATION_HPP__

#include "material_system.hpp"

namespace mhe {

class RenderDataInitializationParameters
{
public:
	void add(const string& name, MaterialSystemContext& context)
	{
		material_systems[name] = context;
	}

	bool get_material_system_parameters(MaterialSystemContext& context, const string& name) const
	{
		hashmap<string, MaterialSystemContext>::const_iterator it = material_systems.find(name);
		if (it == material_systems.end()) return false;
		context = it->value;
		return true;
	}
private:
	hashmap<string, MaterialSystemContext> material_systems;
};

MaterialSystem* create(Context& context, const string& name, const string& instance_name);

template <class T>
T* create(Context& context, const string& name, const string& instance_name)
{
	return checked_static_cast<T*>(create(context, name, instance_name));
}

}

#endif
