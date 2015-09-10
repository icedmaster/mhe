#ifndef __NODE_INSTANCE_METHODS_HPP__
#define __NODE_INSTANCE_METHODS_HPP__

#include "instances.hpp"

namespace mhe {

struct SceneContext;

MHE_EXPORT void set_node_transform(SceneContext& scene_context, NodeInstance& node_instance, const vec3& position, const quatf& rotation, const vec3& scale);

}

#endif
