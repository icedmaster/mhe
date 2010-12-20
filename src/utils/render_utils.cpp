#include "render_utils.hpp"

using namespace mhe;

void Grid::draw()
{
	if (x1_ == x2_ == x3_) return;
	
	// ----------------------------------------
	v3d v1 = (x2_ - x1_);
	v3d v2 = (x3_ - x1_);
	v3d n = cross(v1, v2);
	// get plane coefficients
	float a = n.x(), b = n.y(), c = n.z();
	float d = -(n * v1);
}
