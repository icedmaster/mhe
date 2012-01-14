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
	cmn::uint move_count;
};

class NodeAspect : public Aspect
{
public:
	NodeAspect(const std::string& name, boost::shared_ptr<iNode> node,
			   boost::shared_ptr<Scene> scene) :
		Aspect(name), node_(node), scene_(scene), moved_count_(0), last_time_(0)
	{
		init();
	}

	NodeAspect(const std::string& name, const std::string& add_name,
			   boost::shared_ptr<iNode> node, boost::shared_ptr<Scene> scene) :
		Aspect(name, add_name), node_(node), scene_(scene), moved_count_(0), last_time_(0)
	{
		init();
	}

	~NodeAspect()
	{
		scene_->remove(node_);
	}

	void set(const MoveParams& mp)
	{
		move_params_ = mp;
		cmn::uint now = utils::get_current_tick();
		last_time_ = now;
		moved_count_ = 0;
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
		if (!last_time_) return false;
		if ((tick - last_time_) < move_params_.upd_time) return false;
		if (++moved_count_ > move_params_.move_count)
		{
			update_childs(end_event, this);
			last_time_ = 0;
			return false;
		}
		last_time_ = tick;
		node_->set_transform(move_params_.m * node_->get_transform());
		return true;
	}

	void init()
	{
		scene_->add(node_);
		node_->start();
	}

	boost::shared_ptr<iNode> node_;
	boost::shared_ptr<Scene> scene_;
	cmn::uint moved_count_;
	cmn::uint last_time_;
	MoveParams move_params_;
};

}}

#endif