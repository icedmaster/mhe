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

	Transform(const vec3& position, const quatf& rotation, const vec3& scale) :
		position_(position), rotation_(rotation), scale_(scale)
	{
		update();
	}

	Transform(const mat4x4& transform)
	{
		set(transform);
	}

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
		position_ = transform_.row_vec3(3);
		scale_.set(1, 1, 1);
	}

	void translate_to(const vec3& position)
	{
		position_ = position;
		transform_.set_row(3, position);
	}

	void translate_by(const vec3& delta)
	{
		position_ += delta;
        transform_.set_row(3, position() + delta);
	}

	void rotate_to(const quatf& rotation)
	{
		rotation_ = rotation;
		update();
	}

	void rotate_by(const quatf& rotation)
	{
		rotation_ *= rotation;
		update();
	}

	void scale_to(const vec3& scale)
	{
		scale_ = scale;
		update();
	}

    void scale_by(const vec3& scle)
	{
		scale_ *= scle;
		update();
	}

	const vec3& position() const
	{
		return position_;
	}

	const vec3& scale() const
	{
		return scale_;
	}
private:
	void update()
	{
		transform_ = mat4x4::scaling_matrix(scale_);
		transform_ *= rotation_.normalized().to_matrix<mat4x4>();
		transform_ *= mat4x4::translation_matrix(position_);
	}

	mat4x4 transform_;
	quatf rotation_;
	vec3 position_;
	vec3 scale_;
};

}

#endif
