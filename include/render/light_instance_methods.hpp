#ifndef __LIGHT_INSTANCE_METHODS_HPP__
#define __LIGHT_INSTANCE_METHODS_HPP__

#include "instances.hpp"

namespace mhe {

struct SceneContext;

MHE_EXPORT void set_light_position(SceneContext& scene_context, LightInstance::IdType id, const vec3& position);
MHE_EXPORT const vec3& get_light_position(SceneContext& scene_context, LightInstance::IdType id);

void set_light_direction(SceneContext& scene_context, LightInstance::IdType id, const vec3& direction);
bool get_light_direction(SceneContext& scene_context, LightInstance::IdType id, vec3& direction);

}

#endif
