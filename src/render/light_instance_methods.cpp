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
	ASSERT(scene_context.light_pool.is_valid(id), "Invalid light");
	Transform& transform = scene_context.transform_pool.get(scene_context.light_pool.get(id).transform_id).transform;
	return transform.position();
}

void set_light_direction(SceneContext& scene_context, LightInstance::IdType id, const vec3& direction)
{
}

bool get_light_direction(SceneContext& scene_context, LightInstance::IdType id, vec3& direction);

}
