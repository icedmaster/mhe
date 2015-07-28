#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__

#include "core/config.hpp"
#ifdef RDBG_ENABLED
#include "debug/rdbg.hpp"
#endif
#include "debug/debug_views.hpp"
#include "debug/stats.hpp"

#include "render/context.hpp"
#include "render/render_globals.hpp"
#include "render/renderer.hpp"
#include "events/event_manager.hpp"
#include "scene/scene.hpp"
#include "game_scene.hpp"

#include "utils/sysutils.hpp"

namespace mhe {
namespace game {

class MHE_EXPORT Engine
{
public:
	Engine();
	bool init(uint width, uint height, uint bpp, bool fullscreen);
	void destroy();
	bool init_debug_components();

	Context& context()
	{
		return context_;
	}

	const Context& context() const
	{
		return context_;
	}

	SceneContext& scene_context()
	{
		return scene_.scene_context();
	}

	EventManager& event_manager()
	{
		return event_manager_;
	}

	Scene& scene()
	{
		return scene_;
	}

	const Scene& scene() const
	{
		return scene_;
	}

	RDBGProcessor& rdbg_processor()
	{
		return rdbg_engine_.processor();
	}

	void run();
	void stop();
	void update();
	void render();

	void set_game_scene(GameScene* scene)
	{
		game_scene_ = scene;
	}

	void set_renderer(Renderer* renderer)
	{
		renderer_ = renderer;
	}

	Renderer* renderer()
	{
		return renderer_.get();
	}

	RenderContext& render_context()
	{
		return renderer_->render_context();
	}

	Stats& stats()
	{
		return stats_;
	}
private:
	void set_default_video_settings();
	void setup_generated();

#ifdef RDBG_ENABLED
	RDBGEngine rdbg_engine_;
#endif
	DebugViews debug_views_;
	Stats stats_;

	Context context_;
	EventManager event_manager_;
	Scene scene_;
	utils::Timer stats_timer_;

	ref_ptr<Renderer> renderer_;

	ref_ptr<GameScene> game_scene_;
	bool process_;
};

void init_singletons();
void destroy_singletons();

MHE_EXPORT GlobalVars& get_global_vars();

}}

#endif
