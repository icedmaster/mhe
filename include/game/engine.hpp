#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__

#include "render/context.hpp"
#include "events/event_manager.hpp"
#include "scene/scene.hpp"
#include "game_scene.hpp"

namespace mhe {
namespace game {

class MHE_EXPORT Engine
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

		Scene& scene()
		{
			return scene_;
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

	Context context_;
    EventManager event_manager_;
		Scene scene_;
		ref_ptr<GameScene> game_scene_;
		bool process_;
};

}}

#endif
