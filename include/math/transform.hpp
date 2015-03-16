#ifndef __TRANSFORM_HPP__
#define __TRANSFORM_HPP__

#include "math/matrix.hpp"
#include "math/quat.hpp"

namespace mhe {

class Transform
{
public:
	Transform() : scale_(1.0f, 1.0f, 1.0f)
	{}

	Transform(const vec3& position, const quatf& rotation, const vec3& scale) :
        rotation_(rotation), position_(position), scale_(scale)
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

	void set(const vec3& position, const quatf& rotation, const vec3& scale = vec3(1, 1, 1))
	{
		position_ = position;
		rotation_ = rotation;
		scale_ = scale;
		update();
	}

	void translate_to(const vec3& position)
	{
		position_ = position;
		update();
	}

	void translate_by(const vec3& delta)
	{
		position_ += delta;
        update();
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

	const quatf& rotation() const
	{
		return rotation_;
	}

	const vec3& scale() const
	{
		return scale_;
	}

	mat4x4 view() const
	{
		const vec3& pos = position();
		return (quatf(0.0f, 1.0f, 0.0f, 0.0) * rotation_).to_matrix<mat4x4>() * mat4x4::translation_matrix(-pos);
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
