#ifndef __DEBUG_VIEWS_HPP__
#define __DEBUG_VIEWS_HPP__

#include "imgui_helper.hpp"
#include "core/array.hpp"
#include "render/render_common.hpp"

namespace mhe {

class EventManager;

namespace game {

class Engine;

}

class DebugViews
{
public:
	DebugViews(game::Engine& engine) :
		engine_(engine)
	{}

	void init(EventManager& event_maneger);
	void update();
	void render();
private:
	enum
	{
		posteffect_ssr,
		posteffect_max
	};

	game::Engine& engine_;
	ImGuiHelper imgui_;
	array<MaterialSystemId, posteffect_max> posteffect_id_;
};

}

#endif
