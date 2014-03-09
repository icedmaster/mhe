#include "transform.hpp"

namespace mhe {

void Transform::translate_to(float x, float y, float z)
{
	position_.set(x, y, z);
	update_transform();
}

void Transform::translate_by(const v3d& position)
{
	position_ += position;
	update_transform();
}

void Transform::rotate_to(float angle_x, float angle_y, float angle_z)
{
	rotation_.set_euler(angle_x, angle_y, angle_z);
	update_transform();
}

void Transform::rotate_by(float angle_x, float angle_y, float angle_z)
{
	rotation_ *= quatf(angle_x, angle_y, angle_z);
	update_transform();
}

void Transform::scale_to(float sx, float sy, float sz)
{
	scale_.set(sx, sy, sz);
	update_transform();
}

void Transform::scale_by(float sx, float sy, float sz)
{
	scale_ *= v3d(sx, sy, sz);
	update_transform();
}

void Transform::update_transform()
{
	matrixf t = matrixf::scaling_matrix(scale_);
	t *= rotation_.to_matrix<mat4x4>();
	t *= matrixf::translation_matrix(position_);	
	m_ = t;
}

void Transform::get_transformations()
{
	position_ = m_.row(3).as_vec3();
	// TODO: get rotation
	scale_.set(m_.element(0, 0), m_.element(1, 1), m_.element(2, 2));
}

}
