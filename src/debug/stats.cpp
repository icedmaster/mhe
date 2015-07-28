#include "debug/stats.hpp"
#include "debug/imgui_helper.hpp"

#include "game/engine.hpp"

namespace mhe {

namespace {

size_t render_stats_view_id;
size_t scene_stats_view_id;

}

Stats::Stats()
{}

size_t Stats::register_view(const char* name)
{
	View& view = views_.add();
	view.name = name;
	return views_.size() - 1;
}

size_t Stats::register_field(size_t stats, const char* name, int type)
{
	Field& field = views_[stats].fields.add();
	field.name = name;
	field.type = type;
	::memset(field.value, 0, sizeof(field.value));
	return views_[stats].fields.size() - 1;
}

void Stats::show_gui(size_t view_id) const
{
	const View& view = views_[view_id];
	const Fields& fields = view.fields;
	ImGui::Begin(view.name.c_str());
	for (size_t i = 0, size = fields.size(); i < size; ++i)
		ImGui::LabelText(fields[i].name.c_str(), to_string(fields[i]).c_str());
	ImGui::End();
}

string Stats::to_string(const Field& field) const
{
	switch (field.type)
	{
	case Int:
		return types_cast<string>(TypeHelper<int>::deserialize(field.value));
	default:
		break;
	}

	return string();
}

void init_standart_stat_views(Stats& stats)
{
	render_stats_view_id = stats.register_view("render");
	stats.register_field(render_stats_view_id, "tris", Int);
	stats.register_field(render_stats_view_id, "draw_calls", Int);
	stats.register_field(render_stats_view_id, "fps", Int);

	scene_stats_view_id = stats.register_view("scene");
	stats.register_field(scene_stats_view_id, "aabbs", Int);
	stats.register_field(scene_stats_view_id, "aabbs_visible", Int);
	stats.register_field(scene_stats_view_id, "nodes", Int);
	stats.register_field(scene_stats_view_id, "nodes_visible", Int);
	stats.register_field(scene_stats_view_id, "parts", Int);
	stats.register_field(scene_stats_view_id, "parts_visible", Int);
}

void update_standart_stats_views(Stats& stats, const game::Engine& engine)
{
	const Driver::Stats& render_stats = engine.context().driver.stats();
	if (render_stats.frames() != 0)
	{
		stats.set_current_value(render_stats_view_id, 0, render_stats.tris() / render_stats.frames());
		stats.set_current_value(render_stats_view_id, 1, render_stats.batches() / render_stats.frames());
		stats.set_current_value(render_stats_view_id, 2, render_stats.frames());
	}

	const Scene::Stats& scene_stats = engine.scene().stats();
	stats.set_current_value(scene_stats_view_id, 0, scene_stats.aabbs());
	stats.set_current_value(scene_stats_view_id, 1, scene_stats.aabbs_visible());
	stats.set_current_value(scene_stats_view_id, 2, scene_stats.nodes());
	stats.set_current_value(scene_stats_view_id, 3, scene_stats.nodes_visible());
	stats.set_current_value(scene_stats_view_id, 4, scene_stats.parts());
	stats.set_current_value(scene_stats_view_id, 5, scene_stats.parts_visible());
}

void show_standart_stats_views(Stats& stats)
{
	stats.show_gui(render_stats_view_id);
	stats.show_gui(scene_stats_view_id);
}

}
