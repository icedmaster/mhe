#include "camera.hpp"

#include "video_driver.hpp"

namespace mhe {

Camera::Camera() :
	update_each_frame_(false),
	updated_(false)
{}

void Camera::update(const boost::shared_ptr<Driver>& driver)
{
	if (!updated_)
	{
		driver->set_projection_matrix(transform());
	}
	if (!update_each_frame_) updated_ = true;
}

}
