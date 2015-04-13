#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__

#include "core/config.hpp"
#ifdef RDBG_ENABLED
#include "debug/rdbg.hpp"
#endif

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

	Context& context()
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
private:
	void set_default_video_settings();
	void setup_generated();

#ifdef RDBG_ENABLED
	RDBGEngine rdbg_engine_;
#endif

	Context context_;
    RenderContext render_context_;
	EventManager event_manager_;
	Scene scene_;
	utils::Timer stats_timer_;

	RenderGlobals render_globals_;
	ref_ptr<Renderer> renderer_;

	ref_ptr<GameScene> game_scene_;
	bool process_;
};

}}

#endif
