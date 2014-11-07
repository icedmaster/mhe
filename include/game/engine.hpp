#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__

#include "core/config.hpp"
#ifdef RDBG_ENABLED
#include "debug/rdbg.hpp"
#endif

#include "render/context.hpp"
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
private:
	void set_default_video_settings();
	void setup_generated();

	void update_materials(RenderContext& render_context);

#ifdef RDBG_ENABLED
	RDBGEngine rdbg_engine_;
#endif

	Context context_;
	EventManager event_manager_;
	Scene scene_;
	utils::Timer stats_timer_;

	ref_ptr<GameScene> game_scene_;
	bool process_;
};

}}

#endif
