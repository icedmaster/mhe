#include "camera.hpp"
#include "mhe_gl.hpp"
#include "utils.hpp"

using namespace mhe;

void Camera::set_projection(cmn::uint w, cmn::uint h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = (float)w/(float)h;
    float tan_angle = tan(fov * 3.14159265358f / 180.0);
    glFrustum(-tan_angle * 0.1 * aspect, tan_angle * 0.1 * aspect,
              -tan_angle * 0.1, tan_angle * 0.1, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Camera::update_impl()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(x_ang, x_axis.x(), x_axis.y(), x_axis.z());
    glRotatef(y_ang, y_axis.x(), y_axis.y(), y_axis.z());
    glRotatef(z_ang, z_axis.x(), z_axis.y(), z_axis.z());
    glTranslatef(-pos.x(), -pos.y(), -pos.z());
}


void Camera::rotate_impl(const v3d& v, float ang)
{
    // TODO: need more optimized function
    if (v == x_axis)
        x_ang += ang;
    else if (v == y_axis)
        y_ang += ang;
    else if (v == z_axis)
        z_ang += ang;
    else return;    // not implemented yet
}


void Camera::move_impl(const v3d& v, float dist)
{
    if (v == x_axis)
        pos.set_x(pos.x() + dist);
    else if (v == y_axis)
        pos.set_y(pos.y() + dist);
    else if (v == z_axis)
        pos.set_z(pos.z() + dist);
    else return;    // not implemented yet
}
