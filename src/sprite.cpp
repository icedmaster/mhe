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
	init();
}

void Sprite::draw_impl(const Context& context)
{
	if (!is_running_)
		execute(0);

	Node::draw_impl(context);
}

void Sprite::update_impl(cmn::uint tick)
{
	if (!current_al_) return;
	size_t current_index = current_al_->index();
	AnimationListBase::AnimationChangeType ct = current_al_->update(tick);
	if (ct != AnimationListBase::no_change)
	{
		if (ct == AnimationListBase::last_animation)
		{
			// hack - animation list can be changed in AnimationList delegate
			if (current_index == current_al_->index())
			{
				current_al_ = 0;
				is_alive_ = false;
			}
			return;
		}
		// new frame
		current_al_->update_node(this);  
		update_buffers();
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

void Sprite::add_animation_list(AnimationListBase* al)
{
	bool empty = al_.empty();
	al_[al->index()] = boost::shared_ptr<AnimationListBase>(al);
	if (empty)
		execute(al->index());
}

void Sprite::execute(cmn::uint index)
{
	DEBUG_LOG("Sprite::execute() name=" << name() << " index=" << index);
	is_running_ = true;
	almap::iterator it = al_.find(index);
	if (it == al_.end())
	{
		if (texture() != nullptr)
			update_buffers();
		return;
	}
	current_al_ = it->second.get();
	current_al_->update_node(this);
	update_buffers();
	current_al_->start(utils::get_current_tick());
}

float Sprite::width() const
{
	if (x_size_) return x_size_;
	return texture()->width();
}

float Sprite::height() const
{
	if (y_size_) return y_size_;
	return texture()->height();
}

void Sprite::init()
{
	Node::rvertexcoord().resize(12);
	std::vector<float>& n = Node::rnormalscoord();
	n.resize(12);
	n[0] = 0.0; n[1] = 0.0; n[2] = 1.0;
	n[3] = 0.0; n[4] = 0.0; n[5] = 1.0;
	n[6] = 0.0; n[7] = 0.0; n[8] = 1.0;
	n[9] = 0.0; n[10] = 0.0; n[11] = 1.0;
	
	std::vector<cmn::uint>& i = Node::rindicies();
	i.resize(6);
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 2; i[4] = 3; i[5] = 0;

	std::vector<float>& t = Node::rtexcoord();
	t.resize(8);
	t[0] = 0.0; t[1] = 0.0; t[2] = 0.0; t[3] = 1.0;
	t[4] = 1.0; t[5] = 1.0; t[6] = 1.0; t[7] = 0.0;

	set_mask_z_buffer();
	set_blending_enabled();
	set_blend_mode(alpha_one_minus_alpha);

	DEBUG_LOG("Create sprite " << name() << " with animationLists number:" << al_.size());
}

void Sprite::update_buffers()
{
	std::vector<float>& v = Node::rvertexcoord();
	float x_sz = x_size_, y_sz = y_size_;
	if (!x_size_ || !y_size_)
	{
		x_sz = texture()->width();
		y_sz = texture()->height();
	}

	v[0] = 0.0; v[1] = 0.0; v[2] = 0.0;
	v[3] = 0.0; v[4] = y_sz; v[5] = 0.0;
	v[6] = x_sz; v[7] = y_sz; v[8] = 0.0;
	v[9] = x_sz; v[10] = 0.0; v[11] = 0.0;
	update_color_buffer();
}

}
