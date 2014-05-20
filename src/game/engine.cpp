#include "game/engine.hpp"

#include "game/base_view_events_handler.hpp"
#include "utils/global_log.hpp"

namespace mhe {
namespace game {

bool Engine::init(uint width, uint height, uint bpp, bool fullscreen)
{
	if (!context_.window_system.init(width, height, bpp, fullscreen))
	{
		ERROR_LOG("WindowSystem initialization failed");
		return false;
	}
	INFO_LOG("WindowSystem has been initialized: w=" << width << " h=" << height << " bpp=" << bpp);
	if (!context_.driver.init())
	{
		ERROR_LOG("Driver initialization failed");
		return false;
	}
	INFO_LOG("Driver has been initialized");

    context_.window_system.view()->set_events_handler(new BaseViewEventsHandler(this));

	set_default_video_settings();

	return true;
}

void Engine::set_default_video_settings()
{
	context_.driver.set_viewport(0, 0, context_.window_system.width(), context_.window_system.height());
	context_.driver.set_clear_color(color_black);
	//context_.driver.enable_depth();
}

void Engine::run()
{
	for (;;)
	{
		update();
		render();
	}
}

void Engine::update()
{
    event_manager_.check(context_.window_system);
}

void Engine::render()
{
    context_.driver.clear_color();
    context_.driver.clear_depth();
	context_.driver.begin_render();
	context_.driver.end_render();
    context_.window_system.swap_buffers();
}

}}
