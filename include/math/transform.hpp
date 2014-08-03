#ifndef __TRANSFORM_HPP__
#define __TRANSFORM_HPP__

#include "math/matrix.hpp"
#include "math/quat.hpp"
#include "utils/pool_utils.hpp"

namespace mhe {

class Transform
{
	POOL_ELEMENT_METHODS(uint16_t);
public:
	Transform()
	{}

	Transform(const vec3& position, const quatf& rotation, const vec3& scale)
	{
		update(position, rotation, scale);
	}
private:
	void update(const vec3& position, const quatf& rotation, const vec3& scale)
	{
		transform_ = mat4x4::scaling_matrix(scale);
		transform_ *= rotation.normalized().to_matrix<mat4x4>();
		transform_ *= mat4x4::translation_matrix(position);
	}

	mat4x4 transform_;
};

}

#endif
