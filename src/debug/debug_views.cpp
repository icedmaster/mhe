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
	event_manager.add_bind("standart_stats", keyboard_event_type, KeyboardEvent::key_down, KeyboardDevice::key_f3);
	event_manager.add_bind("debug_ssr", keyboard_event_type, KeyboardEvent::key_down, KeyboardDevice::key_f4);

	imgui_.init(&engine_);

	posteffect_id_[posteffect_ssr] = engine_.context().material_systems.get("ssr")->id();
}

void DebugViews::destroy()
{
	imgui_.destroy();
}

void DebugViews::update()
{
	Renderer* renderer = engine_.renderer();

	Renderer::DebugMode current_mode = renderer->debug_mode();
	Renderer::DebugMode new_mode = current_mode;
	MaterialSystemId material_system_id = InvalidHandle<MaterialSystemId>::id;
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
	else if (engine_.event_manager().check_bind("debug_ssr"))
	{
		if (current_mode == Renderer::renderer_debug_mode_posteffect)
			new_mode = Renderer::renderer_debug_mode_none;
		else
		{
			new_mode = Renderer::renderer_debug_mode_posteffect;
			material_system_id = posteffect_id_[posteffect_ssr];
		}
	}

	if (engine_.event_manager().check_bind("standart_stats"))
		imgui_.toggle_state();

	if (current_mode != new_mode)
		renderer->set_debug_mode(new_mode, material_system_id);

	imgui_.update(engine_.context(), engine_.render_context(), engine_.event_manager());
}

void DebugViews::render()
{
	show_standart_stats_views(engine_.stats());
	imgui_.render(engine_.context(), engine_.render_context());
}

}
