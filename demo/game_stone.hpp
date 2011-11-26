#ifndef __GAME_STONE_HPP__
#define __GAME_STONE_HPP__

#include "mhe.hpp"

struct StoneParameters
{
	std::string name;
	std::string sprite_name;
	mhe::rect<int> pos;
};

mhe::game::aspect_ptr create_stone(const mhe::game::mhe_loader& loader, const StoneParameters& sp);

class StoneAffectorAspect : public mhe::game::Aspect
{
public:
	StoneAffectorAspect(const std::string& name, boost::shared_ptr<mhe::iNode> node,
						boost::shared_ptr<mhe::Scene> scene,
						int on_type = mhe::game::enable_event, int off_type = mhe::game::disable_event,
						const mhe::v3d& pos_correction = mhe::v3d()) :
		mhe::game::Aspect(name), node_(node), scene_(scene),
		on_type_(on_type), off_type_(off_type),
		pos_correction_(pos_correction)
	{}
private:
	void do_subscribe(mhe::game::Aspect* parent)
	{
		parent->subscribe(on_type_, this);
		parent->subscribe(off_type_, this);
	}

	bool update_impl(int type, const void* arg)
	{
		if (type == on_type_)
			process_on_event(type, arg);
		else if (type == off_type_)
			process_off_event(type, arg);
		return true;
	}

	virtual void process_on_event(int /*type*/, const void* arg)
	{
		std::cout << name() << " on\n";
		const mhe::v3d* pos = static_cast<const mhe::v3d*>(arg);
		node_->identity();
		node_->translate(*pos + pos_correction_);
		scene_->add(node_);	
		node_->start();	
	}

	virtual void process_off_event(int /*type*/, const void* /*arg*/)
	{
		std::cout << name() << " off\n";
		scene_->remove(node_);
	}

	boost::shared_ptr<mhe::iNode> node_;
	boost::shared_ptr<mhe::Scene> scene_;
	int on_type_, off_type_;
	mhe::v3d pos_correction_;
}; 

#endif
