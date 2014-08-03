#include "game/input/fps_camera_controller.hpp"

#include "game/engine.hpp"
#include "events/event_manager.hpp"
#include "events/mouse_device.hpp"
#include "events/mouse_event.hpp"
#include "render/render_context.hpp"

namespace mhe {
namespace game {

FPSCameraController::FPSCameraController(Engine& engine, const PerspectiveCameraParameters& parameters,
										 const vec3& position, const vec3& direction, const vec3& up) :
	event_manager_(engine.event_manager())
{
	camera().init(engine.context(), parameters, position, direction, up);
}

void FPSCameraController::update_impl(const RenderContext& render_context)
{
	float time_delta = render_context.fdelta;
	const vec3& side = camera().transform().side_vector();
	const vec3& fwd = camera().transform().forward_vector();
	const vec3& up = camera().transform().up_vector();
	if (event_manager_.check_bind("left"))
		camera().translate_by(side * time_delta * move_speed());
	else if (event_manager_.check_bind("right"))
		camera().translate_by(-side * time_delta * move_speed());
	else if (event_manager_.check_bind("up"))
		camera().translate_by(-fwd * time_delta * move_speed());
	else if (event_manager_.check_bind("down"))
		camera().translate_by(fwd * time_delta * move_speed());

	const MouseDevice* mouse = event_manager_.mouse();
	if (mouse == nullptr) return;

	// translation from mouse wheel
	camera().translate_by(-fwd * time_delta * mouse->wheel_delta().y());

	if (!mouse->is_button_pressed(MouseEvent::right_button)) return;
	const vector2<int>& delta = mouse->delta();
	quatf ry;
	ry.set_rotation(up, deg_to_rad(delta.x()) * rotation_speed() * time_delta);
	quatf rx;
	rx.set_rotation(-side, deg_to_rad(delta.y()) * rotation_speed() * time_delta);
	camera().rotate_by(ry * rx);
}

}}
