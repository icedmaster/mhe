#ifndef __NODE_ASPECT_HPP__
#define __NODE_ASPECT_HPP__

#include "inode.hpp"
#include "aspect.hpp"

namespace mhe {
namespace game {

struct MoveParams
{
	matrixf m;
	cmn::uint upd_time;
	cmn::uint move_time;
};

class NodeAspect : public Aspect
{
public:
	NodeAspect(const std::string& name, boost::shared_ptr<iNode> node) :
		Aspect(name), node_(node), move_end_time_(0)
	{}

	NodeAspect(const std::string& name, const std::string& add_name,
			   boost::shared_ptr<iNode> node) :
		Aspect(name, add_name), node_(node), move_end_time_(0)
	{}

	void set(const MoveParams& mp)
	{
		move_params_ = mp;
		cmn::uint now = utils::get_current_tick();
		move_end_time_ = now + mp.move_time + mp.upd_time;
		last_time_ = now;
	}
private:
	void do_subscribe(Aspect* parent)
	{
		parent->subscribe(transform_event, this);
	}

	void update_impl(cmn::uint tick)
	{
		update_node(tick);
	}

	bool update_impl(int /*type*/, const void* prm)
	{
		const MoveParams* mp = static_cast<const MoveParams*>(prm);
		set(*mp);
		return true;
	}

	bool update_node(cmn::uint tick)
	{
		if (!move_end_time_) return false;
		if ((tick - last_time_) < move_params_.upd_time) return false;
		if (tick > move_end_time_)
		{
			update_childs(end_event, this);
			move_end_time_ = 0;
			return false;
		}
		last_time_ = tick;
		node_->set_transform(move_params_.m * node_->get_transform());
		return true;
	}

	boost::shared_ptr<iNode> node_;
	cmn::uint move_end_time_;
	cmn::uint last_time_;
	MoveParams move_params_;
};

}}

#endif
