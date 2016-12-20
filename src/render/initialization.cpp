#include "render/initialization.hpp"

#include "render/context.hpp"
#include "utils/global_log.hpp"

namespace mhe {

MaterialSystem* create_material_system(Context& context, const string& name, const string& instance_name)
{
    MaterialSystem* material_system = MaterialSystemFactory::instance().create(name.c_str());
    if (material_system == nullptr)
    {
        ERROR_LOG("MaterialSystem initialization failed:" << name << " instance:" << instance_name);
        return nullptr;
    }
    material_system->set_instance_name(instance_name);
    context.material_systems.add(material_system, 0);

    MaterialSystemContext material_context;
    if (!context.initialization_parameters.get_material_system_parameters(material_context, name))
    {
        ERROR_LOG("Can't get initialization params for material " << name);
        return nullptr;
    }

    if (material_context.material_instances_number == 0)
        material_context.material_instances_number = material_system->default_instances_number();
    context.materials[material_system->id()].resize(
        material_context.material_instances_number == 0 ? initial_material_instances_number : material_context.material_instances_number);
    bool res = material_system->init(context, material_context);
    ASSERT(res, "MaterialSystem " << name << " initialization failed");
    material_system->enable();
    material_system->set_priority(material_context.priority);

    return material_system;
}

}
