#ifndef _SPRITE_HPP_
#define _SPRITE_HPP_

#include <vector>
#include <map>
#include "animation_list_base.hpp"
#include "node.hpp"

namespace mhe
{

class Sprite : public Node
{
public:
	Sprite() : is_alive_(true), is_running_(false), x_size_(0.0), y_size_(0.0),
			   current_al_(0), z_order_(0)
	{
		init();
	}

	Sprite(AnimationListBase* al) : is_alive_(true), is_running_(false), x_size_(0.0), y_size_(0.0),
									current_al_(0), z_order_(0)
	{
		al_[al->index()] = boost::shared_ptr<AnimationListBase>(al);
		init();
		execute(0);
	}

	Sprite(const Sprite& sprite);

	void add_animation_list(AnimationListBase* al);
	AnimationListBase* current_animation_list() const
	{
		return current_al_;
	}

	void set_size(float size)
	{
		x_size_ = y_size_ = size;
	}

	void set_size(float xsize, float ysize)
	{
		x_size_ = xsize;
		y_size_ = ysize;
        update_buffers();
	}

	float width() const;
	float height() const;

	size_t get_frames_number() const
	{
		if (current_al_) return current_al_->frames_number();
		return 0;
	}	

	size_t get_animations_number() const
	{
		return al_.size();
	}				

	// execute animation from list with index <index>
	void execute(cmn::uint index);

	void set_z_order(int order)
	{
		z_order_ = order;
	}

	int z_order() const
	{
		return z_order_;
	}
private:
	// implementations
	void draw_impl(Context& context);
	void update_impl(cmn::uint tick);
	void set_position(const v3d& pos);
	matrixf get_matrix() const;
	void start_impl(cmn::uint /*tick*/)
	{
		execute(0);
	}

	Node* clone_impl() const;
private:
	void init();
	void update_buffers();

	typedef std::map < cmn::uint, boost::shared_ptr<AnimationListBase> > almap;
	almap al_;
	bool is_alive_;
	bool is_running_;
	float x_size_, y_size_;
	bool reset_position_;
	v3d pos_;
	AnimationListBase* current_al_;
	int z_order_;
};
}

#endif
