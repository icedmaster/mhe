#ifndef __CAMERA_CONTROLLER_HPP__
#define __CAMERA_CONTROLLER_HPP__

#include "core/ref_counter.hpp"
#include "camera.hpp"

namespace mhe {

struct RenderContext;

class CameraController : public ref_counter
{
public:
	CameraController() :
        move_speed_(0.05f),
        rotation_speed_(0.01f)
	{}

	virtual ~CameraController() {}
	
	void update(const RenderContext& render_context)
	{
		update_impl(render_context);
	}

	const Camera& camera() const
	{
		return camera_;
	}

	Camera& camera()
	{
		return camera_;
	}
protected:
	float move_speed() const
	{
		return move_speed_;
	}

	float rotation_speed() const
	{
		return rotation_speed_;
	}
private:
	virtual void update_impl(const RenderContext& render_context) = 0;

	Camera camera_;
	float move_speed_;
	float rotation_speed_;
};

}

#endif