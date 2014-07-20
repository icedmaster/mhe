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

	Transform(const mat4x4& transform) :
		transform_(transform)
	{}

	operator mat4x4()
	{
		return transform_;
	}

	const mat4x4& transform() const
	{
		return transform_;
	}

	void set(const mat4x4& transform)
	{
		transform_ = transform;
	}

	void translate_to(const vec3& position)
	{
		transform_.set_row(3, position);
	}

	void translate_by(const vec3& delta)
	{
        transform_.set_row(3, position() + delta);
	}

	void rotate_to(const quatf& rotation)
	{
		update(scale(), rotation, position());
	}

	void rotate_by(const quatf& rotation)
	{
		transform_ *= rotation.normalized().to_matrix<mat4x4>();
	}

	void scale_to(const vec3& scale)
	{
		transform_.set_element(0, 0, scale.x());
		transform_.set_element(1, 1, scale.y());
		transform_.set_element(2, 2, scale.z());
	}

    void scale_by(const vec3& scle)
	{
        scale_to(scale() * scle);
	}

	vec3 position() const
	{
        return transform_.row_vec3(3);
	}

	vec3 scale() const
	{
		return vec3(transform_.element(0, 0), transform_.element(1, 1), transform_.element(2, 2));
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
