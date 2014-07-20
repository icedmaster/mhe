#ifndef __NODE_HPP__
#define __NODE_HPP__

#include "render/mesh.hpp"
#include "render/material.hpp"
#include "math/transform.hpp"

namespace mhe {

struct Node
{
	Mesh mesh;
	Transform transform;
	Material material;
};

}

#endif
