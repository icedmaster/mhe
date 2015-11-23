#include "render/node_instance_methods.hpp"

#include "render/scene_context.hpp"

namespace mhe {

void set_node_transform(SceneContext& scene_context, NodeInstance& node_instance, const vec3& position, const quatf& rotation, const vec3& scale)
{
	Transform& transform = scene_context.transform_pool.get(node_instance.transform_id).transform;
	transform.set(position, rotation, scale);
	if (node_instance.aabb_id != AABBInstance::invalid_id)
	{
		AABBf& aabb = scene_context.aabb_pool.get(node_instance.aabb_id).aabb;
		aabb.extents *= scale;
	}
}

}
