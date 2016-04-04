#include "scene/camera.hpp"

#include "render/context.hpp"
#include "render/render_context.hpp"

namespace mhe {

void Camera::init(Context& context, const PerspectiveCameraParameters& parameters,
                  const vec3& position, const vec3& rotation)
{
    position_ = position;
    rotation_.set_euler(rotation.x(), rotation.y(), rotation.z());

    update_view();

    float aspect_ratio = static_cast<float>(context.window_system.width()) / static_cast<float>(context.window_system.height());
    projection_.set_perspective(deg_to_rad(parameters.fov), aspect_ratio, parameters.znear, parameters.zfar);

    znear_ = parameters.znear;
    zfar_ = parameters.zfar;
    float t = ::tan(deg_to_rad(parameters.fov) * 0.5f);
    near_height_ = znear_ * t;
    near_width_ = near_height_ * aspect_ratio;
    far_height_ = zfar_ * t;
    far_width_ = far_height_ * aspect_ratio;
    aspect_ratio_ = aspect_ratio;
    angle_ = deg_to_rad(parameters.fov);

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

void Camera::get(CameraData& camera_data) const
{
    camera_data.camera_frustum = frustum_;
    camera_data.far_height = far_height_;
    camera_data.far_width = far_width_;
    camera_data.near_height = near_height_;
    camera_data.near_width = near_width_;
    camera_data.view = view();
    camera_data.proj = projection();
    camera_data.vp = viewprojection();
    camera_data.viewpos = position();
    camera_data.inv_v = camera_data.view.inverted();
    camera_data.inv_vp = camera_data.vp.inverted();
    camera_data.zfar = zfar_;
    camera_data.znear = znear_;
    camera_data.angle = angle_;
    camera_data.aspect_ratio = aspect_ratio_;
}

void Camera::update_view()
{
    world_ = rotation_.to_matrix<mat4x4>();
    world_.set_row(3, position_);
    view_.set_lookAt(position_, position_ + world_.forward_vector(), vec3::up());
    frustum_.set(viewprojection());
}

}
