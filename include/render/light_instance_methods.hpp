#ifndef __LIGHT_INSTANCE_METHODS_HPP__
#define __LIGHT_INSTANCE_METHODS_HPP__

#include "instances.hpp"

namespace mhe {

struct SceneContext;

MHE_EXPORT void set_light_position(SceneContext& scene_context, LightInstance::IdType id, const vec3& position);
MHE_EXPORT const vec3& get_light_position(SceneContext& scene_context, LightInstance::IdType id);

MHE_EXPORT void set_light_rotation(SceneContext& scene_context, LightInstance::IdType id, const quatf& rotation);

MHE_EXPORT vec3 get_light_direction(SceneContext& scene_context, LightInstance::IdType id);

MHE_EXPORT mat4x4 get_light_shadowmap_matrix(SceneContext& scene_context, LightInstance::IdType id);
MHE_EXPORT mat4x4 get_light_view_matrix(SceneContext& scene_context, LightInstance::IdType id);

}

#endif
