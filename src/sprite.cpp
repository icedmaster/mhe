#include "sprite.hpp"

namespace mhe
{
    // -------------- AnimationList class ---------------------
    AnimationList::AnimationList(bool repeat) :
        autorepeat_(repeat),
        next_tick_(0),
        cur_animation_(0),
        index_(0)
    {
    }

    void AnimationList::reset()
    {
        stop();
    }

    void AnimationList::add(const Animation& a)
    {
        animations_.push_back(a);
    }

    void AnimationList::remove(cmn::uint begin, cmn::uint number)
    {
        if (next_tick_) return; // need to stop animation before removing
        if (animations_.size() <= begin)
            return; // invalid index
        std::vector<Animation>::iterator first = animations_.begin() + begin;
        std::vector<Animation>::iterator last = (animations_.size() >= (begin + number)) ?
                                                first + number :
                                                animations_.end();
        animations_.erase(first, last);
    }

    void AnimationList::start(cmn::uint tick)
    {
        // nothing to animate
        if (animations_.empty()) return;
        next_tick_ = tick + animations_[cur_animation_].duration();
    }

    void AnimationList::stop()
    {
        next_tick_ = 0;
        cur_animation_ = 0;
    }

    bool AnimationList::next()
    {
        if (++cur_animation_ >= animations_.size())
        {
            if (autorepeat_)
                cur_animation_ = 0;
            else
            {
                stop();
                return false;
            }
        }
        if (next_tick_)
            next_tick_ += animations_[cur_animation_].duration();
        return true;
    }

    bool AnimationList::get(Animation& a) const
    {
        if (!next_tick_)    // stopped
            return false;
        a = animations_[cur_animation_];
        return true;
    }

    bool AnimationList::get(cmn::uint tick, Animation& a)
    {
        if (!next_tick_)    // stopped
            return false;
        update(tick);
        a = animations_[cur_animation_];
        return true;
    }

    AnimationList::AnimationChangeType AnimationList::update(cmn::uint tick)
    {
        if ( next_tick_ && (tick >= next_tick_) )
        {
            if (!next())
                return LastAnimation;
            if (!cur_animation_)
                return FirstAnimation;
            return NextAnimation;
        }
        return NoChange;
    }

    //------------------ Sprite class -------------------------
    Sprite::Sprite(const AnimationList& al, const v3d& position) :
        is_alive_(true),
        x_size_(0.0), y_size_(0.0),
        reset_position_(false)
    {
        addAnimationList(al);
        set_position(position);
    }

    void Sprite::draw_impl(const boost::shared_ptr<iDriver>& driver)
    {
        if (!current_al_) return;

        Animation a;
        if (!current_al_->get(a))
        {
            is_alive_ = false;
            return;
        }

		float x_sz = x_size_, y_sz = y_size_;
		if (!x_size_ || !y_size_)
		{
			x_sz = a.texture()->width();
			y_sz = a.texture()->height();
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
        const float t[] = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0};
        const float c[] = {1.0, 1.0, 1.0, 1.0,
                           1.0, 1.0, 1.0, 1.0,
                           1.0, 1.0, 1.0, 1.0,
                           1.0, 1.0, 1.0, 1.0};
        const cmn::uint i[] = {0, 1, 2, 2, 3, 0};

        driver->mask_zbuffer();
        driver->enable_blending(ALPHA_ONE_MINUS_ALPHA);

        a.texture()->prepare();
        driver->draw(m_, v, n, t, c, i, 6);
        a.texture()->clean();

        driver->disable_blending();
        driver->unmask_zbuffer();
    }

    void Sprite::update_impl(cmn::uint tick)
    {
        if (!current_al_) return;
        AnimationList::AnimationChangeType ct = current_al_->update(tick);
        if (ct != AnimationList::NoChange)
        {
            if (ct == AnimationList::LastAnimation)
            {
                current_al_ = 0;
                is_alive_ = false;
                return;
            }
            if ( (ct == AnimationList::FirstAnimation) && reset_position_ )
                set_position(pos_);
            // new frame
            if (current_al_->get(cur_animation))
                m_ = cur_animation.get_transform() * m_;
        }
    }

    void Sprite::set_position(const v3d& pos)
    {
        matrixf tm;
        m_.load_identity();
        tm.set_translate(pos);
        m_ *= tm;
        pos_ = pos;
    }

    matrixf Sprite::get_matrix() const
    {
        return m_;
    }

    void Sprite::addAnimationList(const AnimationList& al)
    {
        al_[al.getIndex()] = al;
    }

    void Sprite::execute(cmn::uint index, cmn::uint tick, bool reset_position)
    {
        almap::iterator it = al_.find(index);
        if (it == al_.end())
            return;
        current_al_ = &(it->second);
        current_al_->reset();
        current_al_->start(tick);
        if (reset_position)
            set_position(pos_);
        // append first transformation
        if (current_al_->get(cur_animation))
            m_ = cur_animation.get_transform() * m_;
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
