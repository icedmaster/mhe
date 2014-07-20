#ifndef __NODE_HPP__
#define __NODE_HPP__

#include "render/mesh.hpp"
#include "render/material.hpp"
#include "render/transform.hpp"

namespace mhe {

struct Node
{
	Mesh mesh;
	uint16_t id;
	Transform::IdType transform;
	MaterialInstance material;
};

}

#endif
