#include "scene/camera.hpp"

#include "render/context.hpp"

namespace mhe {

void Camera::init(Context& context, const PerspectiveCameraParameters& parameters,
				  const vec3& position, const vec3& direction, const vec3& up)
{
	transform_.set_lookAt(position, direction, up);
	position_ = position;

	float aspect_ratio = static_cast<float>(context.window_system.width()) / static_cast<float>(context.window_system.height());
	projection_.set_perspective(deg_to_rad(parameters.fov), aspect_ratio, parameters.znear, parameters.zfar);

    frustum_.set(viewprojection());
}

void Camera::get(mat4x4& v, mat4x4& p, mat4x4& vp) const
{
	v = view();
	p = projection();
	vp = viewprojection();
}

void Camera::update_view()
{
	transform_ = mat4x4::translation_matrix(-position_);
	transform_ *= rotation_.to_matrix<mat4x4>();
	frustum_.set(viewprojection());
}

}
