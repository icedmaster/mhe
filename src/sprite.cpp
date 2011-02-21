#include "sprite.hpp"

namespace mhe
{
	void Sprite::setAnimationList(const anvector& av)
	{
		an_ = av;
		cur_frame_ = 0;
	}

	void Sprite::addAnimation(const Animation& a)
	{
	    a.texture()->setCoord(0, v2d(0, 0));
	    a.texture()->setCoord(1, v2d(0, 1));
	    a.texture()->setCoord(2, v2d(1, 1));
	    a.texture()->setCoord(3, v2d(1, 0));
		an_.push_back(a);
	}

	void Sprite::draw_impl()
	{
		if (an_.empty()) return;

		float w = an_[cur_frame_].texture()->width();
		float h = an_[cur_frame_].texture()->height();

		// draw sprite
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, an_[cur_frame_].texture()->id());
		glBegin(GL_QUADS);
		an_[cur_frame_].texture()->draw_at(0);	glVertex2f(pos_.x(), pos_.y());
		an_[cur_frame_].texture()->draw_at(1);	glVertex2f(pos_.x(), pos_.y() + h);
		an_[cur_frame_].texture()->draw_at(2);	glVertex2f(pos_.x() + w, pos_.y() + h);
		an_[cur_frame_].texture()->draw_at(3);	glVertex2f(pos_.x() + w, pos_.y());
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

	void Sprite::set_next_animation()
	{
		if (!repeat_count_) return;

		if (cur_frame_ == (an_.size() - 1))
			cur_frame_ = 0;
		else
			++cur_frame_;
		next_tick_ += an_[cur_frame_].delay();
		if (repeat_count_ != repeat_forever)
		{
			if (!--repeat_count_)
				is_running_ = false;
		}
	}

	void Sprite::start(cmn::uint tick)
	{
		is_running_ = true;
		next_tick_ = tick + an_[cur_frame_].delay();
	}

	void Sprite::stop()
	{
		is_running_ = false;
	}

	void Sprite::update(cmn::uint tick)
	{
			// change animation if need
		if ( is_running_ && (tick >= next_tick_) )
			set_next_animation();
	}

};
