#include "scene/camera.hpp"

#include "render/context.hpp"

namespace mhe {

Camera::Camera(Context& context, const PerspectiveCameraParameters& parameters,
			   const vec3& position, const vec3& direction, const vec3& up)
{
	mat4x4 m;
	m.set_lookAt(position, direction, up);
	transform_.set(m);

	float aspect_ratio = static_cast<float>(context.window_system.width()) / static_cast<float>(context.window_system.height());
	projection_.set_perspective(parameters.fov, aspect_ratio, parameters.znear, parameters.zfar);
}

void Camera::get(mat4x4& v, mat4x4& p, mat4x4& vp) const
{
	v = view();
	p = projection();
	vp = viewprojection();
}

}
