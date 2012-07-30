#include "sprite.hpp"
#include "utils/sysutils.hpp"
#include "utils/logutils.hpp"

namespace mhe
{

Sprite::Sprite(const Sprite& sprite) :
	al_(sprite.al_), is_alive_(true), is_running_(false),
	x_size_(sprite.x_size_), y_size_(sprite.y_size_),
	reset_position_(sprite.reset_position_),
	pos_(sprite.pos_),
	current_al_(nullptr)
{
}

void Sprite::draw_impl(const Context& context)
{
	if (!is_running_)
		execute(0);
	if (!current_al_) return;

	float x_sz = x_size_, y_sz = y_size_;
	if (!x_size_ || !y_size_)
	{
		x_sz = cur_animation.texture()->width();
		y_sz = cur_animation.texture()->height();
	}

	// prepare buffers for drawing
	const float v[] = {0.0, 0.0, 0.0,
					   0.0, y_sz, 0.0,
					   x_sz, y_sz, 0.0,
					   x_sz, 0.0, 0.0};
	const float n[] = {0.0, 0.0, 1.0,
					   0.0, 0.0, 1.0,
					   0.0, 0.0, 1.0,
					   0.0, 0.0, 1.0};
	const cmn::uint i[] = {0, 1, 2, 2, 3, 0};

	boost::shared_ptr<Driver> driver = context.driver();

	driver->mask_zbuffer();
	driver->enable_blending(ALPHA_ONE_MINUS_ALPHA);

	cur_animation.texture()->prepare();
	driver->draw(get_transform(), v, n, texcoord().data(), color().get(), i, 6);
	cur_animation.texture()->clean();

	driver->disable_blending();
	driver->unmask_zbuffer();
}

void Sprite::update_impl(cmn::uint tick)
{
	if (!current_al_) return;
	AnimationList::AnimationChangeType ct = current_al_->update(tick);
	if (ct != AnimationList::no_change)
	{
		if (ct == AnimationList::last_animation)
		{
			current_al_ = 0;
			is_alive_ = false;
			return;
		}
		// new frame
		current_al_->get(cur_animation);            
	}
}

Node* Sprite::clone_impl() const
{
	Sprite* cloned = new Sprite(*this);
	cloned->is_alive_ = true;
	cloned->is_running_ = false;
	cloned->current_al_ = nullptr;
	return cloned;
}

void Sprite::addAnimationList(const AnimationList& al)
{
	al_[al.index()] = al;
}

void Sprite::execute(cmn::uint index)
{
	DEBUG_LOG("Sprite::execute() name=" << name() << " index=" << index);
	almap::iterator it = al_.find(index);
	if (it == al_.end())
		return;
	current_al_ = &(it->second);
	current_al_->reset();
	current_al_->start(utils::get_current_tick());
	// append first transformation
	current_al_->get(cur_animation);
	is_running_ = true;
}

float Sprite::width() const
{
	if (x_size_) return x_size_;
	return cur_animation.texture()->width();
}

float Sprite::height() const
{
	if (y_size_) return y_size_;
	return cur_animation.texture()->height();
}
}
