#ifndef __FPS_CAMERA_CONTROLLER_HPP__
#define __FPS_CAMERA_CONTROLLER_HPP__

#include "scene/camera_controller.hpp"

namespace mhe {
namespace game {

struct Context;

class FPSCameraController : public CameraController
{
private:
	FPSCameraController(const Context& context, const PerspectiveCameraParameters& parameters,
						const vec3& position, const vec3& direction, const vec3& up);
	void update(const RenderContext& render_context);
};

}}

#endif
