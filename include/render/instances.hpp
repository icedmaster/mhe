#ifndef __AABB_INSTANCE_HPP__
#define __AABB_INSTANCE_HPP__

#include "math/aabb.hpp"
#include "math/transform.hpp"
#include "utils/pool_utils.hpp"
#include "node.hpp"

namespace mhe {

struct AABBInstance
{
	POOL_STRUCT(uint16_t);
	AABBf aabb;
	bool visible;

	AABBInstance() : id(invalid_id) {}
};

struct TransformInstance
{
	POOL_STRUCT(uint16_t);
	Transform transform;
	IdType parent;

	TransformInstance() : id(invalid_id), parent(invalid_id) {}
};

struct NodeInstance
{
	POOL_STRUCT(uint16_t);
	Node node;
	TransformInstance::IdType transform_id;
	AABBInstance::IdType aabb_id;

	NodeInstance() : id(invalid_id), transform_id(TransformInstance::invalid_id), aabb_id(AABBInstance::invalid_id) {}
};

}

#endif
