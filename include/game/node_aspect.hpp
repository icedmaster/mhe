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

inline MoveParams make_move_params(const mhe::matrixf& m, cmn::uint upd_time, cmn::uint move_count)
{
	MoveParams mp;
	mp.m = m;
	mp.upd_time = upd_time;
	mp.move_count = move_count;
	return mp;
}

class NodeAspect : public Aspect
{
public:
	NodeAspect(const std::string& name, boost::shared_ptr<iNode> node,
			   boost::shared_ptr<Scene> scene) :
		Aspect(name), node_(node), scene_(scene), moved_count_(0), last_time_(0),
		current_move_params_(0)
	{
		init();
	}

	NodeAspect(const std::string& name, const std::string& add_name,
			   boost::shared_ptr<iNode> node, boost::shared_ptr<Scene> scene) :
		Aspect(name, add_name), node_(node), scene_(scene), moved_count_(0), last_time_(0),
		current_move_params_(0)
	{
		init();
	}

	~NodeAspect()
	{
		scene_->remove(node_);
	}

	void set(const MoveParams& mp)
	{
		DEBUG_LOG("NodeAspect::set():" << full_name() << " " << move_params_.size());
		if (move_params_.empty())
			init_moving();
		move_params_.push_back(mp);
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

	void init_moving()
	{
		DEBUG_LOG("NodeAspect::init_moving " << full_name() << " " << current_move_params_);
		last_time_ = utils::get_current_tick();
		moved_count_ = 0;
	}

	bool update_node(cmn::uint tick)
	{
		if (!last_time_) return false;
		if ((tick - last_time_) < move_params_[current_move_params_].upd_time) return false;
		if (++moved_count_ > move_params_[current_move_params_].move_count)
		{
			if (++current_move_params_ == move_params_.size())
			{
				DEBUG_LOG("NodeAspect::update_node(): " << full_name() << " end move");
				last_time_ = 0;
				move_params_.clear();
				current_move_params_ = 0;
				update_children(end_event, this);
				return false;
			}
			// set next move params
			init_moving();
			return false;
		}
		last_time_ = tick;
		node_->set_transform(move_params_[current_move_params_].m * node_->get_transform());
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
	std::vector<MoveParams> move_params_;
	size_t current_move_params_;
};

}}

#endif
