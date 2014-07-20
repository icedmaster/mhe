#include "game/input/fps_camera_controller.hpp"

namespace mhe {
namespace game {

FPSCameraController::FPSCameraController(const Context& context, const PerspectiveCameraParameters& parameters,
										 const vec3& position, const vec3& direction, const vec3& up)
{
	camera().init(context, parameters, position, direction, up);
}

void FPSCameraController::update(const RenderContext& render_context)
{
}

}}
