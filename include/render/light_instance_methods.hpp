#ifndef __LIGHT_INSTANCE_METHODS_HPP__
#define __LIGHT_INSTANCE_METHODS_HPP__

#include "instances.hpp"

namespace mhe {

struct SceneContext;

MHE_EXPORT void set_light_position(SceneContext& scene_context, LightInstance::IdType id, const vec3& position);
MHE_EXPORT const vec3& get_light_position(const SceneContext& scene_context, LightInstance::IdType id);

MHE_EXPORT void set_light_rotation(SceneContext& scene_context, LightInstance::IdType id, const quatf& rotation);

MHE_EXPORT vec3 get_light_direction(const SceneContext& scene_context, LightInstance::IdType id);

MHE_EXPORT mat4x4 get_light_shadowmap_matrix(SceneContext& scene_context, LightInstance::IdType id);
MHE_EXPORT mat4x4 get_light_view_matrix(SceneContext& scene_context, LightInstance::IdType id);

void init_light(Context& context, LightInstance& light_instance);
void update_light_uniform(Context& context, SceneContext& scene_context, LightInstance& light_instance);

}

#endif
