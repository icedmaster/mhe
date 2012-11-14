#ifndef __CAMERA_2D_HPP__
#define __CAMERA_2D_HPP__

#include "camera.hpp"

namespace mhe {

class Camera2D : public Camera
{
public:
	Camera2D(float width, float height)
	{
		matrixf proj;
		proj.set_ortho(0, width, 0, height, -1, 1);
		set_transform(proj);
	}
};

}

#endif
