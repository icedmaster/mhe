#ifndef _VIEWPORT_HPP_
#define _VIEWPORT_HPP_

#include "types.hpp"
#include "mhe_gl.hpp"

namespace mhe
{
	class Viewport
	{
		private:
			cmn::uint x_, y_, w_, h_;
			
			virtual void setup_impl();
		public:
			Viewport() :
				x_(0), y_(0), w_(0), h_(0)
			{}
			
			Viewport(cmn::uint x, cmn::uint y,
					 cmn::uint w, cmn::uint h) :
				x_(x), y_(y), w_(w), h_(h)
			{}
			
			void set(cmn::uint x, cmn::uint y,
					 cmn::uint w, cmn::uint h)
			{
				x_ = x; y_ = y;
				w_ = w; h_ = h;
			}
			
			cmn::uint w() const
			{
				return w_;
			}
			
			cmn::uint h() const
			{
				return h_;
			}
			
			void setup()
			{
				setup_impl();
			}
	};
};

#endif
