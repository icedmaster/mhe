#ifndef __STATS_COMPONENT_HPP__
#define __STATS_COMPONENT_HPP__

#include "scene.hpp"
#include "component.hpp"
#include "gui/label.hpp"
#include "gui/gui_node.hpp"

namespace mhe {
namespace game {
namespace utils {

class StatsComponent : public Component
{
public:
	StatsComponent(const boost::shared_ptr<Scene>& scene, Engine* engine) :
		Component("globals"),
		label_(nullptr),
		prev_tick_(0)
	{
		init(scene, engine);
	}

	~StatsComponent()
	{
		scene_->parent()->remove_subscene(scene_.get());
	}
private:
	std::string add_name_impl() const
	{
		return "stats";
	}

	void do_subscribe(Component* /*component*/) {}
	bool update_impl(const Message& /*message*/) 
	{
		return false;
	}

	void update_impl(cmn::uint tick, Engine* engine);
private:
	void init(const boost::shared_ptr<Scene>& scene, Engine* engine);

	boost::shared_ptr<Scene> scene_;
	gui::Label* label_;
	cmn::uint prev_tick_;
};

}}}

#endif
