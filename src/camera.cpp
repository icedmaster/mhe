#include "camera.hpp"

#include "video_driver.hpp"

namespace mhe {

Camera::Camera() :
	update_each_frame_(false),
	updated_(false)
{
}

Camera::Camera(const vector3<float>& position, const vector3<float>& direction,
			   const vector3<float>& up) :
	update_each_frame_(false), updated_(false)
{
	set(position, direction, up);
}

void Camera::set(const vector3<float>& position, const vector3<float>& direction,
				 const vector3<float>& up)
{
	look_.set_lookAt(position, direction, up);
	set_transform(look_);	
}

void Camera::update(Driver& driver)
{
	update_view_matrix();
	update_projection(driver);
}

void Camera::update_view_matrix()
{
	if (dirty())
	{
		updated_ = false;
		// recalculate new transform
		vector3<float> pos = position();
		set_position(-pos);
		update_transform();
		set_position(pos);
		set_transform(look_ * transform());
		clear_dirty_flag();
	}
}

void Camera::update_projection(Driver& driver)
{
	if (!updated_)
	{
		driver.set_projection_matrix(transform() * projection_);
	}
	if (!update_each_frame_) updated_ = true;
}

}
