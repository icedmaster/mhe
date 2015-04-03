#ifndef __AABB_INSTANCE_HPP__
#define __AABB_INSTANCE_HPP__

#include "math/aabb.hpp"
#include "math/transform.hpp"
#include "utils/pool_utils.hpp"
#include "node.hpp"
#include "light.hpp"

namespace mhe {

struct AABBInstance
{
	POOL_STRUCT(uint16_t);
	AABBf aabb;
    bool visible : 1;
    bool enabled : 1;

    AABBInstance() : id(invalid_id), enabled(true) {}
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
    MeshInstance mesh;
	TransformInstance::IdType transform_id;
	AABBInstance::IdType aabb_id;
    bool cast_shadow : 1;
    bool receive_shadow : 1;
    bool enabled : 1;

    NodeInstance() : id(invalid_id), transform_id(TransformInstance::invalid_id), aabb_id(AABBInstance::invalid_id),
    cast_shadow(true), receive_shadow(true), enabled(true)
    {}
};

struct LightInstance
{
	POOL_STRUCT(uint16_t);
	Light light;
	TransformInstance::IdType transform_id;
	AABBInstance::IdType aabb_id;
	bool enabled;

	LightInstance() : id(invalid_id), transform_id(TransformInstance::invalid_id), aabb_id(AABBInstance::invalid_id), enabled(true) {}
};

}

#endif
