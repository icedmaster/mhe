#include "cube.hpp"

using namespace mhe;

void Cube::createPolygons()
{
    // front
    faces[0].set( v3d(ll_.x(), ll_.y(), ll_.z()),
                  v3d(ll_.x(), rh_.y(), ll_.z()),
                  v3d(rh_.x(), rh_.y(), ll_.z()),
                  v3d(rh_.x(), ll_.y(), ll_.z()) );
    // right
    faces[1].set( v3d(rh_.x(), ll_.y(), ll_.z()),
                  v3d(rh_.x(), rh_.y(), ll_.z()),
                  v3d(rh_.x(), rh_.y(), rh_.z()),
                  v3d(rh_.x(), ll_.y(), rh_.z()) );
    // back
    faces[2].set( v3d(rh_.x(), ll_.y(), rh_.z()),
                  v3d(rh_.x(), rh_.y(), rh_.z()),
                  v3d(ll_.x(), rh_.y(), rh_.z()),
                  v3d(ll_.x(), ll_.y(), rh_.z()) );
    // left
    faces[3].set( v3d(ll_.x(), ll_.y(), rh_.z()),
                  v3d(ll_.x(), rh_.y(), rh_.z()),
                  v3d(ll_.x(), rh_.y(), ll_.z()),
                  v3d(ll_.x(), ll_.y(), ll_.z()) );
    // top
    faces[4].set( v3d(ll_.x(), rh_.y(), ll_.z()),
                  v3d(ll_.x(), rh_.y(), rh_.z()),
                  v3d(rh_.x(), rh_.y(), rh_.z()),
                  v3d(rh_.x(), rh_.y(), ll_.z()) );
    // bottom
    faces[5].set( v3d(ll_.x(), ll_.y(), ll_.z()),
                  v3d(rh_.x(), ll_.y(), ll_.z()),
                  v3d(rh_.x(), ll_.y(), rh_.z()),
                  v3d(ll_.x(), ll_.y(), rh_.z()) );
}

void Cube::draw_impl()
{
    for (int i = 0; i < 6; ++i)
        faces[i].draw();
}

