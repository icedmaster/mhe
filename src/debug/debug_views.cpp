#include "debug/debug_views.hpp"

#include "events/event_manager.hpp"
#include "events/keyboard_device.hpp"
#include "events/keyboard_event.hpp"

#include "render/renderer.hpp"
#include "game/engine.hpp"

namespace mhe {

void DebugViews::init(EventManager& event_manager)
{
	event_manager.add_bind("debug_main", keyboard_event_type, KeyboardEvent::key_down, KeyboardDevice::key_l);
}

void DebugViews::update()
{
	Renderer* renderer = engine_.renderer();

	if (engine_.event_manager().check_bind("debug_main"))
	{
		if (renderer->debug_mode() == Renderer::renderer_debug_mode_main)
			renderer->set_debug_mode(Renderer::renderer_debug_mode_none);
		else renderer->set_debug_mode(Renderer::renderer_debug_mode_main);
	}
}

}
