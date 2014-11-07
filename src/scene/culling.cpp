#include "scene/culling.hpp"

namespace mhe {

void frustum_culling(const frustumf& frustum, AABBInstance* aabbs, size_t number)
{
	const planef* planes = frustum.planes();
	for (size_t i = 0; i < number; ++i)
	    aabbs[i].visible = is_inside(planes, aabbs[i].aabb);
}

bool is_inside(const planef* planes, const AABBf& aabb)
{
	return true;
}

}
