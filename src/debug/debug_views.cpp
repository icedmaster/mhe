#include "debug/debug_views.hpp"

#include "events/event_manager.hpp"
#include "events/keyboard_device.hpp"
#include "events/keyboard_event.hpp"

#include "render/renderer.hpp"
#include "game/engine.hpp"

namespace mhe {

void DebugViews::init(EventManager& event_manager)
{
	event_manager.add_bind("debug_main", keyboard_event_type, KeyboardEvent::key_down, KeyboardDevice::key_f1);
	event_manager.add_bind("debug_shadow", keyboard_event_type, KeyboardEvent::key_down, KeyboardDevice::key_f2);
}

void DebugViews::update()
{
	Renderer* renderer = engine_.renderer();

	Renderer::DebugMode current_mode = renderer->debug_mode();
	Renderer::DebugMode new_mode = current_mode;
	if (engine_.event_manager().check_bind("debug_main"))
	{
		if (current_mode == Renderer::renderer_debug_mode_main)
			new_mode = Renderer::renderer_debug_mode_none;
		else new_mode = Renderer::renderer_debug_mode_main;
	}
	else if (engine_.event_manager().check_bind("debug_shadow"))
	{
		if (current_mode == Renderer::renderer_debug_mode_shadows)
			new_mode = Renderer::renderer_debug_mode_none;
		else new_mode = Renderer::renderer_debug_mode_shadows;
	}

	if (current_mode != new_mode)
		renderer->set_debug_mode(new_mode);
}

}
