#ifndef _TRANSFORM_HPP_
#define _TRANSFORM_HPP_

#include "mhe_math.hpp"

namespace mhe
{
class Transform
{
public:
	Transform() :
		scale_(1, 1, 1)
	{}

	Transform(const mat4x4& nm) :
		m_(nm),
		scale_(1, 1, 1)
	{}

	void set_transform(const mat4x4& nm)
	{
		m_ = nm;
		get_transformations();
	}

	const mat4x4& transform() const
	{
		return m_;
	}

	void apply_transform(const mat4x4& nm)
	{
		m_ *= nm;
		get_transformations();
	}

	void rotate(const v3d& v, float angle)
	{
		rotation_.set_rotation(v, deg_to_rad(angle));
		update_transform();
	}

	void set_identity()
	{
		set_transform(matrixf::identity());
		position_.set(0, 0, 0);
		rotation_ = quatf::identity();
		scale_.set(1, 1, 1);
	}

	void translate_to(const v3d& position)
	{
		translate_to(position.x(), position.y(), position.z());
	}

	void translate_to(float x, float y, float z);
	void translate_by(const v3d& position);
	void rotate_to(float angle_x, float angle_y, float angle_z);
	void rotate_to(const vector3<float>& angles)
	{
		rotate_to(angles.x(), angles.y(), angles.z());
	}
	void rotate_by(float angle_x, float angle_y, float angle_z);
	void scale_to(float sx, float sy, float sz);
	void scale_to(const vector3<float>& s)
	{
		scale_to(s.x(), s.y(), s.z());
	}
	void scale_by(float sx, float sy, float sz);

	const vector3<float>& position() const
	{
		return position_;
	}

	vec3 rotation() const
	{
		return rotation_.euler<vec3>();
	}

	const vector3<float>& scaling() const
	{
		return scale_;
	}
protected:
	void update_transform();
	void get_transformations();

	void set_position(const vector3<float>& position)
	{
		translate_to(position);
	}
private:
	mat4x4 m_;   
	quatf rotation_;
	vector3<float> position_; 
	vector3<float> scale_;
};
}

#endif
