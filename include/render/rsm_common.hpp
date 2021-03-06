#ifndef __RSM_COMMON_HPP__
#define __RSM_COMMON_HPP__

#include "math/matrix.hpp"

namespace mhe {

struct RSMData
{
    mat4x4 vp;
    vec3 light_direction;
    AABBf aabb;
};

}

#endif
