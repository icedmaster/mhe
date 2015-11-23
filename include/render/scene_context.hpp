#ifndef __SCENE_CONTEXT_HPP__
#define __SCENE_CONTEXT_HPP__

#include "instances.hpp"
#include "core/config.hpp"
#include "core/pool.hpp"

namespace mhe {

typedef Pool< TransformInstance, max_scene_nodes_number, TransformInstance::IdType, StructTypePolicy<TransformInstance> > TransformPool;
typedef Pool< NodeInstance, max_scene_nodes_number, NodeInstance::IdType, StructTypePolicy<NodeInstance> > NodePool;

typedef Pool< AABBInstance, max_scene_nodes_number, AABBInstance::IdType, StructTypePolicy<AABBInstance> > AABBPool;

typedef Pool< LightInstance, max_lights_number, LightInstance::IdType, StructTypePolicy<LightInstance> > LightPool;

struct SceneContext
{
	TransformPool transform_pool;
	AABBPool aabb_pool;
	AABBPool parts_aabb_pool;
	NodePool node_pool;
	LightPool light_pool;
};

}

#endif
