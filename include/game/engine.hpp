#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__

#include "render/context.hpp"
#include "events/event_manager.hpp"

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

    EventManager& event_manager()
    {
        return event_manager_;
    }

	void run();
	void update();
	void render();
private:
	void set_default_video_settings();

	Context context_;
    EventManager event_manager_;
};

}}

#endif
