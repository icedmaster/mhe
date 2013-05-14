#ifndef __CAMERA2D_HPP__
#define __CAMERA2D_HPP__

#include "camera.hpp"

namespace mhe {

class Camera2D : public Camera
{
public:
	Camera2D(float width, float height)
	{
		set_size(width, height);
	}

	void set_size(float width, float height)
	{
		matrixf proj;
		proj.set_ortho(0, width, 0, height, -1, 1);
		set_transform(proj);
		set_dirty();
	}
};

}

#endif
