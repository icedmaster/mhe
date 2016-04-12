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

void serialize_light_settings(uint8_t*& stream, const Light::Settings& settings)
{
    serialize(stream, settings.light_desc.directional.directional_unused1);
    serialize(stream, settings.light_desc.directional.directional_unused2);
    serialize(stream, settings.light_desc.directional.directional_unused3);
    serialize(stream, settings.light_desc.directional.directional_unused4);
    serialize(stream, settings.light_desc.directional.directional_shadowmap_projection_width);
    serialize(stream, settings.light_desc.directional.directional_shadowmap_projection_height);
    serialize(stream, settings.light_desc.directional.directional_shadowmap_projection_znear);
    serialize(stream, settings.light_desc.directional.directional_shadowmap_projection_zfar);
    serialize(stream, settings.light_desc.shadowmap_bias);
    serialize(stream, settings.light_desc.cast_shadows);
    serialize(stream, settings.light_desc.auto_shadow_configuration);

    serialize(stream, settings.shading_settings.diffuse);
    serialize(stream, settings.shading_settings.specular);
    serialize(stream, settings.shading_settings.intensity);
    serialize(stream, settings.shading_settings.specular_intensity);
}

void serialize_light_instance(uint8_t* stream, const LightInstance& light_instance)
{
    serialize(stream, light_instance.id);
    serialize(stream, light_instance.transform_id);
    serialize(stream, light_instance.aabb_id);
    serialize(stream, light_instance.enabled);
    serialize(stream, light_instance.light.type());
    serialize_light_settings(stream, light_instance.light.settings());
}

size_t light_settings_serialization_data_size()
{
    return sizeof(float) * 9 + sizeof(bool) * 2 +
           sizeof(vec4) * 2 + sizeof(float) * 2;
}

size_t light_instance_serialization_data_size()
{
    return sizeof(LightInstance::IdType) + sizeof(TransformInstance::IdType) +
        sizeof(AABBInstanceHandleType) + sizeof(bool) + sizeof(int) + light_settings_serialization_data_size();
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
    uint32_t objects_number = engine.scene_context().light_pool.size();

    ASSERT(size > objects_number * light_instance_serialization_data_size() + sizeof(uint32_t), "Invalid buffer size");
    size = objects_number * light_instance_serialization_data_size() + sizeof(uint32_t);

    serialize(data, objects_number);

    for (size_t i = 0; i < objects_number; ++i)
    {
        serialize_light_instance(data, light_instances[i]);
    }
    return true;
}

bool light_set_object_data(game::Engine& engine, uint32_t objectid, const uint8_t* data, size_t size, uint32_t field)
{
    ASSERT(field < 4, "Invalid field index");
    SetObjectFieldDataFunc funcs[4] = {nullptr, nullptr, nullptr, light_set_enabled_flag};
    SetObjectFieldDataFunc current_func = funcs[field];
    if (current_func == nullptr) return true;
    return current_func(engine, objectid, data, size);
}

bool light_get_object_data(game::Engine& engine, uint32_t objectid, uint8_t* data, size_t& size, size_t offset)
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

bool light_set_enabled_flag(game::Engine& engine, size_t id, const uint8_t* data, size_t size)
{
    if (!engine.scene_context().light_pool.is_valid(id))
        return false;
    engine.scene_context().light_pool.get(id).enabled = deserialize<bool>(data, size);
    return true;
}

}
