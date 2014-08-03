#ifndef __FPS_CAMERA_CONTROLLER_HPP__
#define __FPS_CAMERA_CONTROLLER_HPP__

#include "scene/camera_controller.hpp"
#include "core/compiler.hpp"

namespace mhe {

class EventManager;

namespace game {

class Engine;

class MHE_EXPORT FPSCameraController : public CameraController
{
public:
		FPSCameraController(Engine& engine, const PerspectiveCameraParameters& parameters,
						const vec3& position, const vec3& direction, const vec3& up);
private:
	void update_impl(const RenderContext& render_context);

	const EventManager& event_manager_;
};

}}

#endif
