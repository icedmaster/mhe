#include "render/light_instance_methods.hpp"

#include "render/scene_context.hpp"
#include "render/uniforms.hpp"
#include "render/context.hpp"
#include "scene/scene.hpp"
#include "res/serialize.hpp"

namespace mhe {

namespace {
mat4x4 update_light_transform(const LightInstance& light, const vec3& position)
{
    if (light.dblight.type == directional) return mat4x4::identity();
    if (light.dblight.type == omni)
    {
        mat4x4 res = mat4x4::scaling_matrix(light.dblight.radius);
        res.set_row(3, position);
        return res;
    }
    return mat4x4::identity();
}
}

LightInstance& create_light(Context& context, Scene& scene, const FilePath& filename)
{
    LightInstance& light_instance = scene.create_light(directional); // the actual type will be loaded from the file
    light_instance.resource_path = filename;
    reload_light(context, scene, light_instance);
    return light_instance;
}

void reload_light(Context&, Scene& scene, LightInstance& light_instance)
{
    res::XMLDeserializer deserializer(light_instance.resource_path.c_str());
    {
        deserializer.begin_field("light");
        bool res = light_instance.dblight.read(deserializer);
        ASSERT(res == true, "Can't read serialized data from:" << light_instance.resource_path);
        deserializer.end_field();
    }
    {
        deserializer.begin_field("transform");
        res::Transform dbtransform;
        bool res = dbtransform.read(deserializer);
        deserializer.end_field();
        ASSERT(res == true, "Can't read serialized data from:" << light_instance.resource_path);
        Transform& transform = scene.transform_pool().get(light_instance.transform_id).transform;
        transform.set(dbtransform.position, dbtransform.rotation, dbtransform.scale);
    }
}

bool serialize_light(Context&, Scene& scene, LightInstance& light_instance, const FilePath& filename)
{
    res::XMLSerializer serializer(filename.c_str());
    // TODO: add VERIFY macro
    {
        serializer.begin_field("light");
        bool res = light_instance.dblight.write(serializer);
        ASSERT(res == true, "Can't write serialized data to:" << light_instance.resource_path);
        serializer.end_field();
    }
    {
        Transform& transform = scene.transform_pool().get(light_instance.transform_id).transform;
        res::Transform dbtransform;
        dbtransform.position = transform.position();
        dbtransform.rotation = transform.rotation();
        dbtransform.scale = transform.scale();
        serializer.begin_field("transform");
        bool res = dbtransform.write(serializer);
        serializer.end_field();
        ASSERT(res == true, "Can't write serialized data to:" << light_instance.resource_path);
    }
    return true;
}

void set_light_position(SceneContext& scene_context, LightInstance::IdType id, const vec3& position)
{
    LightInstance& light = scene_context.light_pool.get(id);
    Transform& transform = scene_context.transform_pool.get(light.transform_id).transform;
    transform.translate_to(position);
    AABBf& aabb = scene_context.aabb_pool.get(light.aabb_id).aabb;
    aabb.center = position;
}

const vec3& get_light_position(const SceneContext& scene_context, LightInstance::IdType id)
{
    Transform& transform = scene_context.transform_pool.get(scene_context.light_pool.get(id).transform_id).transform;
    return transform.position();
}

void set_light_rotation(SceneContext& scene_context, LightInstance::IdType id, const quatf& rotation)
{
    LightInstance& light = scene_context.light_pool.get(id);
    Transform& transform = scene_context.transform_pool.get(light.transform_id).transform;
    transform.rotate_to(rotation);
}

void set_light_direction(SceneContext& scene_context, LightInstance::IdType id, const vec3& direction, const vec3& up)
{
    LightInstance& light_instance = scene_context.light_pool.get(id);
    Transform& transform = scene_context.transform_pool.get(light_instance.transform_id).transform;
    mat4x4 m;
    const vec3& pos = transform.position();
    m.set_direction(pos, direction, up);
    transform.set(m);
}

vec3 get_light_direction(const SceneContext& scene_context, LightInstance::IdType id)
{
    LightInstance& light_instance = scene_context.light_pool.get(id);
    Transform& transform = scene_context.transform_pool.get(light_instance.transform_id).transform;
    const mat4x4& m = transform.transform();
    return m.forward_vector();
}

mat4x4 get_light_shadowmap_matrix(SceneContext& scene_context, LightInstance::IdType id)
{
    const LightInstance& light_instance = scene_context.light_pool.get(id);
    const res::Light& light = light_instance.dblight;
    ASSERT(light.cast_shadows, "get_light_shadowmap_matrix() called for light source with incorrect cast_shadow flag");
    NOT_IMPLEMENTED_ASSERT(light.type != omni, "Omni shadowmaps");
    mat4x4 proj;
    if (light.type == spot)
        proj.set_perspective(light.angle * 2.0f, 1.0f, light.shadowmap_znear, light.shadowmap_zfar);
    
    Transform& transform = scene_context.transform_pool.get(light_instance.transform_id).transform;
    return transform.view() * proj;
}

mat4x4 get_light_view_matrix(SceneContext& scene_context, LightInstance::IdType id)
{
    const LightInstance& light_instance = scene_context.light_pool.get(id);
    Transform& transform = scene_context.transform_pool.get(light_instance.transform_id).transform;
    return transform.view();
}

void init_light(Context& context, LightInstance& light_instance)
{
    UniformBufferDesc uniform_buffer_desc;
    uniform_buffer_desc.size = light_instance.dblight.type == directional ? sizeof(DirectionalLightData) : sizeof(LightData);
    uniform_buffer_desc.unit = 0; // can be any valid slot
    uniform_buffer_desc.update_type = uniform_buffer_normal;
    UniformBuffer& uniform = create_and_get(context.uniform_pool);
    bool res = uniform.init(uniform_buffer_desc);
    ASSERT(res, "Light uniform buffer initialization failed");
    light_instance.uniform_id = uniform.id();
}

void update_light_uniform(Context& context, SceneContext& scene_context, LightInstance& light_instance)
{
    res::Light& light = light_instance.dblight;
    UniformBuffer& uniform_buffer = context.uniform_pool.get(light_instance.uniform_id);
    if (light.type != directional)
    {
        LightData light_data;
        light_data.diffuse = vec4(light.diffuse_color * light.intensity, light.angle);
        light_data.specular = vec4(light.specular_color * light.specular_intensity, light.attenuation);
        const vec3& light_position = get_light_position(scene_context, light_instance.id);
        light_data.position = vec4(light_position.x(), light_position.y(), light_position.z(), light.attenuation);
        const vec3& light_direction = get_light_direction(scene_context, light_instance.id);
        light_data.direction = vec4(light_direction.x(), light_direction.y(), light_direction.z(), light.spot_attenuation);

        const ShadowInfo* shadow_info = light_instance.shadow_info;
        if (shadow_info != nullptr)
        {
            light_data.lightvp = shadow_info->lightvp[0];
            light_data.shadowmap_params = vec4(light.shadowmap_bias, 0.0f, 0.0f, 0.0f);
        }

        light_data.lightw = update_light_transform(light_instance, light_position);

        uniform_buffer.update(light_data);
    }
    else
    {
        DirectionalLightData light_data;
        light_data.diffuse = vec4(light.diffuse_color * light.intensity, 0.0f);
        light_data.specular = vec4(light.specular_color * light.specular_intensity, 0.0f);
        light_data.direction = get_light_direction(scene_context, light_instance.id);

        const ShadowInfo* shadow_info = light_instance.shadow_info;
        if (shadow_info != nullptr)
        {
            light_data.shadowmap_params = vec4(light.shadowmap_bias, 0.0f, 0.0f, 0.0f);

            light_data.cascades_number = shadow_info->cascades_number;
            for (size_t i = 0; i < shadow_info->cascades_number; ++i)
            {
                light_data.csm_offset[i] = shadow_info->offset[i];
                light_data.csm_scale[i] = shadow_info->scale[i];
                light_data.cascade_znear[i] = shadow_info->znear[i];
                light_data.cascade_zfar[i] = shadow_info->zfar[i];
                light_data.lightvp[i] = shadow_info->lightview[i];
                light_data.scsm_params[i] = shadow_info->scsm_params[i];
            }
        }

        uniform_buffer.update(light_data);
    }
}

}
