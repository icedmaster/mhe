#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__

#include "render/context.hpp"

namespace mhe {
namespace game {

class Engine
{
public:
	bool init(uint width, uint height, uint bpp, bool fullscreen);

	Context& context()
	{
		return context_;
	}

	void run();
	void update();
	void render();
private:
	void set_default_video_settings();

	Context context_;
};

}}

#endif
