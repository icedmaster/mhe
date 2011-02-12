#include "viewport.hpp"

namespace mhe
{
	void Viewport::setup_impl()
	{
		glViewport(x_, y_, w_, h_);
	}
};
