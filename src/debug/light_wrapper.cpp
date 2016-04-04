#include "debug/light_wrapper.hpp"

#include "core/config.hpp"
#include "game/engine.hpp"
#include "render/instances.hpp"

#ifdef RDBG_ENABLED
#include "debug/rdbg.hpp"
#endif

#include "debug/serialization.hpp"

namespace mhe {

namespace {

void serialize_light_instance(uint8_t* stream, const LightInstance& light_instance)
{
    serialize(stream, light_instance.id);
    serialize(stream, light_instance.transform_id);
    serialize(stream, light_instance.aabb_id);
    serialize(stream, light_instance.enabled);
    serialize(stream, light_instance.light.settings());
}

size_t light_instance_serialization_data_size()
{
    return sizeof(LightInstance::IdType) + sizeof(TransformInstance::IdType) +
        sizeof(AABBInstanceHandleType) + sizeof(bool) + sizeof(Light::Settings);
}

}

void register_light_type(RDBGProcessor& rdbg_processor)
{
#ifdef RDBG_ENABLED
    TypeRegistrationInfo reginfo;
    reginfo.type_name = "light";
    reginfo.set_data_func = light_set_data;
    reginfo.get_data_func = light_get_data;
    reginfo.set_object_data_func = light_set_object_data;
    reginfo.get_object_data_func = light_get_object_data;
    rdbg_processor.register_type(reginfo);
#endif
}

bool light_set_data(game::Engine& engine, const uint8_t* data, size_t size, size_t offset)
{
    NOT_IMPLEMENTED_METHOD();
    return false;
}

bool light_get_data(game::Engine& engine, uint8_t* data, size_t& size, size_t offset)
{
    LightInstance* light_instances = engine.scene_context().light_pool.all_objects();
    size_t objects_number = engine.scene_context().light_pool.size();

    ASSERT(size > objects_number * light_instance_serialization_data_size(), "Invalid buffer size");
    size = objects_number * light_instance_serialization_data_size();

    for (size_t i = 0; i < objects_number; ++i)
    {
        serialize_light_instance(data, light_instances[i]);
    }
    return true;
}

bool light_set_object_data(game::Engine& engine, size_t objectid, const uint8_t* data, size_t size, size_t offset)
{
    return false;
}

bool light_get_object_data(game::Engine& engine, size_t objectid, uint8_t* data, size_t& size, size_t offset)
{
    return false;
}

bool set_diffuse_color(game::Engine& engine, size_t id, const colorf& color)
{
    if (!engine.scene_context().light_pool.is_valid(id))
        return false;
    engine.scene_context().light_pool.get(id).light.shading().diffuse = color;
    return true;
}

bool get_diffuse_color(game::Engine& engine, size_t id, colorf& color)
{
    if (!engine.scene_context().light_pool.is_valid(id))
        return false;
    color = engine.scene_context().light_pool.get(id).light.shading().diffuse;
    return true;
}

bool set_specular_color(game::Engine& engine, size_t id, const colorf& color)
{
    if (!engine.scene_context().light_pool.is_valid(id))
        return false;
    engine.scene_context().light_pool.get(id).light.shading().specular = color;
    return true;
}

bool get_specular_color(game::Engine& engine, size_t id, colorf& color)
{
    if (!engine.scene_context().light_pool.is_valid(id))
        return false;
    color = engine.scene_context().light_pool.get(id).light.shading().specular;
    return true;
}

}
