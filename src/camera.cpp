#include "camera.hpp"

#include "video_driver.hpp"

namespace mhe {

Camera::Camera() :
	update_each_frame_(false),
	updated_(false)
{}

void Camera::update(Driver& driver)
{
	if (dirty())
	{
		updated_ = false;
		// recalculate new transform
		matrixf current = transform();
		update_transform();
		set_transform(current * transform());
		clear_dirty_flag();
	}
	if (!updated_)
	{
		driver.set_projection_matrix(transform() * projection_);
	}
	if (!update_each_frame_) updated_ = true;
}

}
