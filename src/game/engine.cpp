#include "game/engine.hpp"

#include "game/base_view_events_handler.hpp"
#include "utils/global_log.hpp"
#include "utils/sysutils.hpp"

#include "render/render_context.hpp"

#ifdef RDBG_ENABLED
#include "debug/default_rdbg_setup.hpp"
#endif

#include "debug/profiler.hpp"

namespace mhe {
namespace game {

Engine::Engine()
#ifdef RDBG_ENABLED
	:
	rdbg_engine_(*this)
#endif
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
					ERROR_LOG("All available drivers has been tried - can't initialize engine");
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
	INFO_LOG("Driver has been initialized");

  context_.window_system.view()->set_events_handler(new BaseViewEventsHandler(this));

	context_.shader_manager.set_context(&context_);
	context_.mesh_manager.set_context(&context_);
	context_.texture_manager.set_context(&context_);

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
	PROFILE("engine.update");
    event_manager_.check(context_.window_system);
		if (game_scene_ != nullptr)
			game_scene_->update(*this);

		RenderContext render_context;
        render_context.tick = utils::get_current_time();
				render_context.fdelta = utils::get_last_delta();
		SceneContext scene_context;
		scene_.update(render_context, context_, scene_context);
		update_materials(render_context);
}

void Engine::render()
{
	PROFILE("engine.render");
    context_.driver.clear_color();
    context_.driver.clear_depth();
	context_.driver.begin_render();

	Node* nodes = nullptr;
	size_t count = scene_.nodes(nodes);
	context_.driver.render(context_, nodes, count);
	if (game_scene_ != nullptr)
		game_scene_->draw(*this);
	context_.driver.end_render();
    context_.window_system.swap_buffers();
}

void Engine::update_materials(RenderContext& render_context)
{
	size_t indexes[4096];
	scene_.transform_pool().all_indexes(indexes, sizeof(indexes));
	const MaterialSystems::Values &systems = context_.material_systems.get_all_materials();
	for (size_t i = 0; i < systems.size(); ++i)
	{
		Node* nodes = nullptr;
		size_t offset;
		size_t count = scene_.nodes(nodes, offset, systems[i]->id());
		if (!count) continue;
		systems[i]->update(context_, render_context, nodes, scene_.transform_pool().all_objects(), indexes, count);
	}
}

}}
