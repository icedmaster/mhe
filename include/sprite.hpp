#ifndef _SPRITE_HPP_
#define _SPRITE_HPP_

#include <vector>
#include <boost/shared_ptr.hpp>
#include "geom.hpp"
#include "itexture.hpp"
#include "globals.hpp"
#include "irenderable.hpp"

namespace mhe
{
	class Animation
	{
		private:
			cmn::uint delay_;
			boost::shared_ptr<iTexture> tex_;
		public:
			Animation(cmn::uint delay,
					  const boost::shared_ptr<iTexture>& t) :
				delay_(delay),
				tex_(t)
			{}

			void set_delay(cmn::uint delay)
			{
				delay_ = delay;
			}

			cmn::uint delay() const
			{
				return delay_;
			}

			const boost::shared_ptr<iTexture>& texture() const
			{
				return tex_;
			}
	};

	typedef std::vector< Animation > anvector;

	class Sprite : public iRenderable
	{
		static const int repeat_forever = -1;
		private:
			anvector an_;
			size_t cur_frame_;
			cmn::uint id_;

			bool is_running_;
			int repeat_count_;
			cmn::uint next_tick_;
			v2d pos_;

			void set_next_animation();
		private:
			cmn::uint num_vertexes() const
			{
				return 4;
			}

			int get_id() const
			{
				return id_;
			}

			void set_position(const v3d& v)
			{
				pos_.set(v.x(), v.y());
			}

			void draw_impl();
		public:
			Sprite() :
				cur_frame_(0),
				//id_(globals::instance().get_id()),
				is_running_(false)
			{}

			Sprite(cmn::uint id) :
				cur_frame_(0),
				id_(id),
				is_running_(false)
			{}

			void setAnimationList(const anvector& av);
			void addAnimation(const Animation& a);

			void start(cmn::uint tick);
			void stop();

			void update(cmn::uint tick);

			void setPosition(const v2d& p)
			{
			    set_position(v3d(p.x(), p.y(), 0));
			}
	};
};

#endif
