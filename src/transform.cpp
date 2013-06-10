#include "transform.hpp"

namespace mhe {

void Transform::translate_to(const v3d& position)
{
	position_ = position;
	set_dirty();
}

void Transform::translate_by(const v3d& position)
{
	position_ += position;
	set_dirty();
}

void Transform::rotate_to(float angle_x, float angle_y, float angle_z)
{
	rotation_.set(angle_x, angle_y, angle_z);
	set_dirty();
}

void Transform::rotate_by(float angle_x, float angle_y, float angle_z)
{
	rotation_ += v3d(angle_x, angle_y, angle_z);
	set_dirty();
}

void Transform::scale_to(float sx, float sy, float sz)
{
	scaling_.set(sx, sy, sz);
	set_dirty();
}

void Transform::scale_by(float sx, float sy, float sz)
{
	scaling_ *= v3d(sx, sy, sz);
	set_dirty();
}

void Transform::update_transform()
{
	identity();
	matrixf t = matrixf::scaling_matrix(scaling_);
	t *= matrixf::rotation_matrix(deg_to_rad(-rotation_.x()), deg_to_rad(-rotation_.y()), deg_to_rad(-rotation_.z()));
	t *= matrixf::translation_matrix(position_);	
	set_transform(t);
}

}
