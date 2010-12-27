#include "light.hpp"

using namespace mhe;

Light::Light() :
    gl_num(0),
    is_enable_(false),
    lt(LightDirected)
{
    set_position(v3d(0.0, 10, 0));
}


Light::Light(int num, LightType t) :
    diffuse_color(colorf(1.0, 1.0, 1.0, 1.0)),
    gl_num(num),
    is_enable_(false),
    lt(t)
{
    set_position(v3d(0, 10, 0));
    set_light_type(t);
}


Light::Light(const Light& l) :
    pos(l.pos),
    dir(l.dir),
    diffuse_color(l.diffuse_color),
    gl_num(l.gl_num),
    is_enable_(l.is_enable_),
    lt(l.lt)
{
}


void Light::update_impl()
{
    glLightfv(GL_LIGHT0 + gl_num, GL_DIFFUSE, diffuse_color.get());
    glLightfv(GL_LIGHT0 + gl_num, GL_POSITION, pos.get());
    glLightfv(GL_LIGHT0 + gl_num, GL_SPOT_DIRECTION, dir.get());
}
