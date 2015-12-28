#include "debug/debug_views.hpp"

#include "events/event_manager.hpp"
#include "events/keyboard_device.hpp"
#include "events/keyboard_event.hpp"

#include "render/renderer.hpp"
#include "game/engine.hpp"

#include "imgui/imgui.h"

namespace mhe {

void DebugViews::init(EventManager& event_manager)
{
	event_manager.add_bind("debug_main", keyboard_event_type, KeyboardEvent::key_pressed, KeyboardDevice::key_f1);
	event_manager.add_bind("debug_shadow", keyboard_event_type, KeyboardEvent::key_pressed, KeyboardDevice::key_f2);
	event_manager.add_bind("standart_stats", keyboard_event_type, KeyboardEvent::key_pressed, KeyboardDevice::key_f3);
	event_manager.add_bind("debug_ssr", keyboard_event_type, KeyboardEvent::key_pressed, KeyboardDevice::key_f4);
	event_manager.add_bind("debug_ssao", keyboard_event_type, KeyboardEvent::key_pressed, KeyboardDevice::key_f5);
	event_manager.add_bind("debug_baked_irradiance", keyboard_event_type, KeyboardEvent::key_pressed, KeyboardDevice::key_f6);

	stats_enabled_ = false;

	imgui_.init(&engine_);

	MaterialSystem* material_system = engine_.context().material_systems.get("ssr");
	if (material_system != nullptr)
		posteffect_id_[posteffect_ssr] = material_system->id();
	material_system = engine_.context().material_systems.get("ssao");
	if (material_system != nullptr)
		posteffect_id_[posteffect_ssao] = material_system->id();
	posteffect_debug_mode_ = posteffect_max;
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
		if (current_mode == Renderer::renderer_debug_mode_posteffect && posteffect_debug_mode_ == posteffect_ssr)
		{
			new_mode = Renderer::renderer_debug_mode_none;
			posteffect_debug_mode_ = posteffect_max;
		}
		else
		{
			new_mode = Renderer::renderer_debug_mode_posteffect;
			posteffect_debug_mode_ = posteffect_ssr;
			material_system_id = posteffect_id_[posteffect_ssr];
		}
	}
	else if (engine_.event_manager().check_bind("debug_ssao"))
	{
		if (current_mode == Renderer::renderer_debug_mode_posteffect && posteffect_debug_mode_ == posteffect_ssao)
		{
			new_mode = Renderer::renderer_debug_mode_none;
			posteffect_debug_mode_ = posteffect_max;
		}
		else
		{
			new_mode = Renderer::renderer_debug_mode_posteffect;
			posteffect_debug_mode_ = posteffect_ssao;
			material_system_id = posteffect_id_[posteffect_ssao];
		}
	}
	else if (engine_.event_manager().check_bind("debug_baked_irradiance"))
	{
		if (current_mode == Renderer::renderer_debug_mode_baked_irradiance)
		{
			new_mode = Renderer::renderer_debug_mode_none;
			posteffect_debug_mode_ = posteffect_max;
		}
		else
		{
			new_mode = Renderer::renderer_debug_mode_baked_irradiance;
		}
	}

	if (engine_.event_manager().check_bind("standart_stats"))
		stats_enabled_ ^= 1;

	if (current_mode != new_mode)
		renderer->set_debug_mode(new_mode, material_system_id);

	imgui_.update(engine_.context(), engine_.render_context(), engine_.event_manager());

	for (size_t i = 0, size = debug_views_.size(); i < size; ++i)
		debug_views_[i].update();
}

void DebugViews::render()
{
	if (stats_enabled_)
		show_standart_stats_views(engine_.stats());
	imgui_.render(engine_.context(), engine_.render_context());
}

size_t DebugViews::add_view(const string& name)
{
	size_t id = debug_views_.size();
	DebugView& view = debug_views_.add();
	view.set_name(name);
	return id;
}

DebugViews::DebugView& DebugViews::get_view(size_t id)
{
	ASSERT(id < debug_views_.size(), "Invalid debug view id:" << id);
	return debug_views_[id];
}

void DebugViews::DebugView::add(const string& name, float min_value, float max_value, float* value)
{
	FloatDebugField* field = new FloatDebugField;
	field->name = name;
	field->min_value = min_value;
	field->max_value = max_value;
	field->value = value;

	fields_.push_back(ref_ptr<DebugField>(field));
}

void DebugViews::DebugView::update()
{
	ImGui::Begin(name_.c_str());
	for (size_t i = 0, size = fields_.size(); i < size; ++i)
	{
		fields_[i]->update();
	}
	ImGui::End();
}

void DebugViews::FloatDebugField::update()
{
	ImGui::SliderFloat(name.c_str(), value, min_value, max_value);
}

}
