#include "animation_list.hpp"

namespace mhe {

AnimationList::AnimationList(size_t index, bool repeat) :
	AnimationListBase(index, repeat),
	cur_animation_(0)
{
}

void AnimationList::reset()
{
	stop();
}

void AnimationList::add(Animation* a)
{
	animations_.push_back(boost::shared_ptr<Animation>(a));		
}

void AnimationList::remove(cmn::uint begin, cmn::uint number)
{
	if (is_running()) return;
	if (animations_.size() <= begin)
		return; // invalid index
	std::vector< boost::shared_ptr<Animation> >::iterator first = animations_.begin() + begin;
	std::vector< boost::shared_ptr<Animation> >::iterator last = (animations_.size() >= (begin + number)) ?
		first + number :
		animations_.end();
	animations_.erase(first, last);
}

bool AnimationList::next()
{
	if (++cur_animation_ >= animations_.size())
		return false;
	return true;
}

void AnimationList::start_impl()
{
	cur_animation_ = 0;
}

void AnimationList::update_node_impl(Node* node)
{
	animations_[cur_animation_]->update(node);
}

Animation* AnimationList::current_animation() const
{
	return animations_[cur_animation_].get();
}

}
