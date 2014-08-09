#ifndef __UNIFORMS_HPP__
#define __UNIFORMS_HPP__

#include "math/matrix.hpp"

namespace mhe {

struct TransformSimpleData
{
	mat4x4 vp;
};

struct TransformData
{
	mat4x4 vp;
};

struct PerModelSimpleData
{
	mat4x4 model;
};

struct PerModelData
{
	mat4x4 model;
	mat4x4 normal;
};

}

#endif
