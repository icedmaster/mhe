#ifndef __ANIMATION_LIST_HPP__
#define __ANIMATION_LIST_HPP__

#include <vector>
#include "animation_list_base.hpp"
#include "animation.hpp"

namespace mhe {

class AnimationList : public AnimationListBase
{
public:
	AnimationList(size_t index, bool repeat = false);

	void reset();
	void add(Animation* a);
	void remove(cmn::uint begin, cmn::uint number);

	Animation* current_animation() const;
private:
	bool next();
	size_t current_frame_duration_impl() const
	{
		return animations_[cur_animation_]->duration();
	}

	size_t frames_number_impl() const
	{
		return animations_.size();
	}

	void start_impl();
	void update_node_impl(Node* node);
private:
	std::vector< boost::shared_ptr<Animation> > animations_;
	size_t cur_animation_;
};

}

#endif
