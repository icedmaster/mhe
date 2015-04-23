#include "game/engine.hpp"

#include "game/base_view_events_handler.hpp"
#include "utils/global_log.hpp"

#include "render/render_context.hpp"
#include "render/renderer.hpp"

#ifdef RDBG_ENABLED
#include "debug/default_rdbg_setup.hpp"
#endif

#include "debug/profiler.hpp"

namespace mhe {
namespace game {

Engine::Engine()
#ifdef RDBG_ENABLED
	:
    rdbg_engine_(*this),
#endif
    scene_(context_)
{}

bool Engine::init(uint width, uint height, uint bpp, bool fullscreen)
{
	process_ = false;

		while (true)
		{
			WindowContextFormat format;
			format.major_version = context_.driver.major_version_need();
			format.minor_version = context_.driver.minor_version_need();
			if (!context_.window_system.init(width, height, bpp, fullscreen, format))
			{
				ERROR_LOG("Can't initialize WindowSystem with format:" << format.major_version << " " <<
					format.minor_version);
				const std::string& name = SystemFactory::instance().video_driver_factory().set_next_driver();
				if (name.empty())
				{
					ERROR_LOG("All available drivers have been tried - can't initialize engine");
					return false;
				}
				INFO_LOG("Try next driver with name:" << name);
				context_.driver.reset();
				continue;
			}
			else
			{
				INFO_LOG("Initialized WindowSystem with format:" << format.major_version << " " <<
					format.minor_version);
				break;
			}
	}

	if (!context_.driver.init())
	{
		ERROR_LOG("Driver initialization failed");
		return false;
	}
	context_.driver.set_window_size(context_.window_system.screen_size());
	INFO_LOG("Driver has been initialized");

  context_.window_system.view()->set_events_handler(new BaseViewEventsHandler(this));

	context_.shader_manager.set_context(&context_);
	context_.mesh_manager.set_context(&context_);
	context_.texture_manager.set_context(&context_);
	context_.material_manager.set_context(&context_);

	set_default_video_settings();

	setup_generated();

#ifdef RDBG_ENABLED
	if (!rdbg_engine_.start())
	{
		WARN_LOG("RDBG engine initialization failed");
	}
	else
	{
		INFO_LOG("RDBG engine started");
	}
	default_rdbg_setup(rdbg_engine_.processor());
#endif

	stats_timer_.start();

	setup(render_globals_);

	return true;
}

void Engine::set_default_video_settings()
{
	context_.driver.set_viewport(0, 0, context_.window_system.width(), context_.window_system.height());
	context_.driver.set_clear_color(color_black);
}

void Engine::run()
{
	process_ = true;
	while (process_)
	{
		ProfilerElement pe("engine.run");
		update();
		render();
	}
}

void Engine::stop()
{
	process_ = false;
}

void Engine::update()
{
	MainProfiler::instance().clear();
	ProfilerElement pe("engine.update");

	if (use_vsync.reset_if_changed())
	{
		if (use_vsync.value())
			context_.window_system.enable_vsync();
		else context_.window_system.disable_vsync();
	}

	if (stats_timer_.elapsed() > 1.0f)
	{
		context_.driver.stats().reset();
		stats_timer_.start();
	}

	event_manager_.check(context_.window_system);
	if (game_scene_ != nullptr)
		game_scene_->update(*this);

    render_context_.tick = utils::get_current_time();
    render_context_.fdelta = utils::get_last_delta();
    scene_.update(render_context_);
	renderer_->update(render_context_, scene_.scene_context());
}

void Engine::render()
{
	if (game_scene_ != nullptr)
		game_scene_->before_draw(*this);
	ProfilerElement pe("engine.render");
	renderer_->render(render_context_, scene_.scene_context());

	if (game_scene_ != nullptr)
		game_scene_->draw(*this);

	renderer_->flush();
	
    render_context_.draw_calls.clear();
}

}}
