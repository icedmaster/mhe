#include "game/stats_component.hpp"

#include "camera2d.hpp"
#include "utils/types_cast.hpp"
#include "utils/graphics_utils.hpp"
#include "utils/global_log.hpp"
#include "game/engine.hpp"

namespace mhe {
namespace game {
namespace utils {

void StatsComponent::init(const boost::shared_ptr<Scene>& scene, Engine* engine)
{
	scene_.reset(new Scene);
	gui::GUINode* node = new gui::GUINode;
	label_ = new gui::Label;
	label_->set_sprite(mhe::utils::create_sprite(color_transparent, vector2<float>(32, 32), engine->context()));
	label_->set_font(engine->font_manager().get("droid-sans.fnt"));
	label_->set_geom(rect<float>(0, engine->context().window_system().height() - 50, 80, 50));
	label_->set_caption_color(color_white);
	node->set_widget(label_);
	scene_->add(nodeptr(node));
	scene_->add_camera(new Camera2D(engine->context().window_system().width(),
									engine->context().window_system().height()), true);
	scene_->active_camera()->enable_each_frame_updating(true);
	scene->add_subscene(scene_);
}

void StatsComponent::update_impl(cmn::uint tick, Engine* engine)
{
	if (tick - prev_tick_ < 1000) return;
	unsigned int fps = engine->context().driver().stats().frames();
	std::string s = "fps:" + mhe::utils::types_cast<std::string>(fps) +
		" tris:" + mhe::utils::types_cast<std::string>(engine->context().driver().stats().tris() / fps) +
		" dips:" + mhe::utils::types_cast<std::string>(engine->context().driver().stats().batches() / fps);
	label_->set_caption(mhe::utils::utf8_to_utf32(s));
	prev_tick_ = tick;
	s += (" r:" + mhe::utils::types_cast<std::string>(engine->context().driver().stats().elements_count() / fps));
	DEBUG_LOG("stats:" << s);
	engine->context().driver().stats().reset();
}

}}}
