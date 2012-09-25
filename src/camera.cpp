#include "camera.hpp"

namespace mhe
{
	Camera::Camera() :
		pos_(0.0, -10.0, 0.0),
		id(0)
	{
		// init default settings
		setPerspective(45.0, 4.0/3.0, 0.1, 100);
		// look at
		set_lookAt();
	}

	void Camera::set_lookAt()
	{
		matrixf m;
		m.set_lookAt(pos_, dir_, y_axis);
		Transform::set_transform(m);
	}

	void Camera::setPosition(const v3d& pos)
	{
		pos_ = pos;
		set_lookAt();
	}

	void Camera::setDirection(const v3d& dir)
	{
		dir_ = dir;
		set_lookAt();
	}

	void Camera::setPerspective(float fov, float aspect,
								float znear, float zfar)
	{
		proj_m.set_perspective(fov, aspect, znear, zfar);
	}

	void Camera::update(boost::shared_ptr<Driver> driver)
	{
		// update matricies
		driver->set_projection_matrix(proj_m);
		driver->set_modelview_matrix(Transform::transform());
	}
}
