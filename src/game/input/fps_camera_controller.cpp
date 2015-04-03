#include "game/input/fps_camera_controller.hpp"

#include "game/engine.hpp"
#include "events/event_manager.hpp"
#include "events/mouse_device.hpp"
#include "events/mouse_event.hpp"
#include "render/render_context.hpp"

namespace mhe {
namespace game {

FPSCameraController::FPSCameraController(Engine& engine, const PerspectiveCameraParameters& parameters,
										 const vec3& position, const vec3& rotation) :
	CameraController(engine.scene()),
	event_manager_(engine.event_manager()),
	angles_(rotation)
{
	camera().init(engine.context(), parameters, position, rotation);
	transform_instance().transform.set(position, camera().rotation());
}

FPSCameraController::FPSCameraController(Engine& engine, const PerspectiveCameraParameters& parameters,
										 const vec3& position, const vec3& direction, const vec3& up) :
	CameraController(engine.scene()),
	event_manager_(engine.event_manager())
{
	// TODO: need to implement correct behaviour
	camera().init(engine.context(), parameters, position, vec3::zero());
	transform_instance().transform.set(position, camera().rotation());
}

void FPSCameraController::update_impl(const RenderContext& render_context)
{
	float time_delta = render_context.fdelta;
	const vec3& side = camera().right();
	const vec3& fwd = camera().forward();

	vec3 velocity;
	if (event_manager_.check_bind("left"))
		velocity = side * time_delta * move_speed();
	else if (event_manager_.check_bind("right"))
		velocity = -side * time_delta * move_speed();
	else if (event_manager_.check_bind("up"))
		velocity = fwd * time_delta * move_speed();
	else if (event_manager_.check_bind("down"))
		velocity = -fwd * time_delta * move_speed();

	const MouseDevice* mouse = event_manager_.mouse();
	if (mouse == nullptr) return;

	// translation from mouse wheel
	if (mouse->wheel_delta().y() > 0)
		camera().translate_by(fwd * time_delta * mouse->wheel_delta().y());

	quatf rotation;
	float xangle = 0.0f;
	float yangle = 0.0f;
	if (mouse->is_button_pressed(MouseEvent::right_button))
	{
		const vector2<int>& delta = mouse->delta();
		xangle = deg_to_rad(static_cast<float>(delta.y())) * rotation_speed() * time_delta;
		yangle = deg_to_rad(static_cast<float>(delta.x())) * rotation_speed() * time_delta;
	}
	quatf yaw;
	quatf pitch;
	float x = angles_.x() + xangle;
	float y = angles_.y() + yangle;
	angles_.set(x, y, 0.0f);
	pitch.set_rotation(vec3::right(), x);
	yaw.set_rotation(vec3::up(), y);

	rotation = pitch * yaw;
	
	Transform& transform = transform_instance().transform;
	transform.translate_by(velocity);
	transform.rotate_to(rotation);
	sync_camera();
}

}}
