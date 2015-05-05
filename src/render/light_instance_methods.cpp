#include "render/light_instance_methods.hpp"

#include "render/scene_context.hpp"

namespace mhe {

void set_light_position(SceneContext& scene_context, LightInstance::IdType id, const vec3& position)
{
    LightInstance& light = scene_context.light_pool.get(id);
    Transform& transform = scene_context.transform_pool.get(light.transform_id).transform;
	transform.translate_to(position);
    AABBf& aabb = scene_context.aabb_pool.get(light.aabb_id).aabb;
    aabb.center = position;
}

const vec3& get_light_position(SceneContext& scene_context, LightInstance::IdType id)
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

vec3 get_light_direction(SceneContext& scene_context, LightInstance::IdType id)
{
	LightInstance& light_instance = scene_context.light_pool.get(id);
	Transform& transform = scene_context.transform_pool.get(light_instance.transform_id).transform;
	const mat4x4& m = transform.transform();
	return m.forward_vector();
}

mat4x4 get_light_shadowmap_matrix(SceneContext& scene_context, LightInstance::IdType id)
{
	const LightInstance& light_instance = scene_context.light_pool.get(id);
	const Light& light = light_instance.light;
	const LightDesc& desc = light.desc();
	ASSERT(desc.cast_shadows, "get_light_shadowmap_matrix() called for light source with incorrect cast_shadow flag");
	NOT_IMPLEMENTED_ASSERT(light.type() != Light::omni, "Omni shadowmaps");
	mat4x4 proj;
	if (light.type() == Light::directional)
	{
		float w = desc.directional.directional_shadowmap_projection_width * 0.5f;
		float h = desc.directional.directional_shadowmap_projection_height * 0.5f;
		proj.set_ortho(-w, w, -h, h, desc.directional.directional_shadowmap_projection_znear, desc.directional.directional_shadowmap_projection_zfar);
	}
	else if (light.type() == Light::spot)
		proj.set_perspective(desc.spot.angle * 2.0f, 1.0f, desc.spot.spot_shadowmap_projection_znear, desc.spot.spot_shadowmap_projection_zfar);
	
	Transform& transform = scene_context.transform_pool.get(light_instance.transform_id).transform;
	return transform.view() * proj;
}

mat4x4 get_light_view_matrix(SceneContext& scene_context, LightInstance::IdType id)
{
	const LightInstance& light_instance = scene_context.light_pool.get(id);
	Transform& transform = scene_context.transform_pool.get(light_instance.transform_id).transform;
	return transform.view();
}

}
