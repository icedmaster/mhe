#include "camera.hpp"
#include "mhe_gl.hpp"
#include "utils.hpp"

using namespace mhe;

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

    /*
    utils::truncate<float>(x_ang, 0, 360);
    utils::truncate<float>(y_ang, 0, 360);
    utils::truncate<float>(z_ang, 0, 360);
    */
}
