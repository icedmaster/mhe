#ifndef __CULLING_HPP__
#define __CULLING_HPP__

#include "math/frustum.hpp"
#include "math/aabb.hpp"
#include "render/instances.hpp"

namespace mhe {

void frustum_culling(const frustumf& frustum, AABBInstance* aabbs, size_t number);
bool is_inside(const planef* planes, const AABBf& aabb); 

}

#endif
