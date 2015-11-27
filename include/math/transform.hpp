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
		static quatf zaxis_rotation(0.0f, pi, 0.0f);
		mat4x4 res =  transform_;
		res.set_row(3, vec3::zero());
		res *= zaxis_rotation.to_matrix<mat4x4>();
		res.transpose();
		res.multTranslate(-position_);
		return res;
	}

	mat4x4 inverse_transform() const
	{
		mat4x4 res;
		res = mat4x4::scaling_matrix(vec3(1.0f / scale_.x(), 1.0f / scale_.y(), 1.0f / scale_.z()));
		res *= rotation_.conjugate().to_matrix<mat4x4>();
		res *= mat4x4::translation_matrix(-position_);
		return res;
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
