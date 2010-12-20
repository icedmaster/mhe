#ifndef _RENDER_UTILS_HPP_
#define _RENDER_UTILS_HPP_

#include "irenderable.hpp"

namespace mhe
{
	class Grid : public iRenderable
	{
		private:
			v3d x1_, x2_, x3_;
			float step_;
		public:
			Grid(const v3d& x1, const v3d& x2, const v3d& x3, float step = 1.0) :
				x1_(x1), x2_(x2), x3_(x3)
				step_(step)
			{}
			
			void draw();
	};
	
};

#endif
