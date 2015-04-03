#include "scene/camera.hpp"

#include "render/context.hpp"

namespace mhe {

void Camera::init(Context& context, const PerspectiveCameraParameters& parameters,
				  const vec3& position, const vec3& rotation)
{
	position_ = position;
	rotation_.set_euler(rotation.x(), rotation.y(), rotation.z());

	update_view();

	float aspect_ratio = static_cast<float>(context.window_system.width()) / static_cast<float>(context.window_system.height());
	projection_.set_perspective(deg_to_rad(parameters.fov), aspect_ratio, parameters.znear, parameters.zfar);

    frustum_.set(viewprojection());
}

void Camera::set(const vec3& position, const quatf& rotation)
{
	position_ = position;
	rotation_ = rotation;
	rotation_.normalize();
	update_view();
}

void Camera::get(mat4x4& v, mat4x4& p, mat4x4& vp) const
{
	v = view();
	p = projection();
	vp = viewprojection();
}

void Camera::update_view()
{
	world_ = rotation_.to_matrix<mat4x4>();
	world_.set_row(3, position_);
	view_.set_lookAt(position_, position_ + world_.forward_vector(), vec3::up());
	frustum_.set(viewprojection());
}

}
