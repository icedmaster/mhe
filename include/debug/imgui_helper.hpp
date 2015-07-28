#ifndef __IMGUI_HELPER_HPP__
#define __IMGUI_HELPER_HPP__

#include "imgui/imgui.h"
#include "render/render_common.hpp"
#include "render/render_buffer.hpp"
#include "render/render_state.hpp"
#include "render/texture.hpp"
#include "math/vector4.hpp"
#include "math/vector2.hpp"

namespace mhe {

struct Context;
struct RenderContext;
class EventManager;

namespace game {

class Engine;

}

class MHE_EXPORT ImGuiHelper
{
public:
	struct Vertex
	{
		vec4 pos;
		vec4 col;
		vec2 tex;
	};

	bool init(game::Engine* engine);
	void destroy();

	void update(Context& context, RenderContext& render_context, const EventManager& event_manager);
	void render(Context& context, RenderContext& render_context);

	void enable()
	{
		enabled_ = true;
	}

	void disable()
	{
		enabled_ = false;
	}

	void toggle_state()
	{
		enabled_ ^= 1;
	}
private:
	bool init_layout(Context& context);
	bool init_render_state(Context& context);
	bool init_buffers(Context& context);
	bool init_uniform(Context& context);
	bool init_imgui(Context& context);

	void update_uniform(UniformBuffer& uniform_buffer, Context& context);

	ShaderProgramHandleType shader_program_id_;
	TextureHandleType font_texture_id_;
	LayoutHandleType layout_id_;
	RenderStateHandleType render_state_id_;
	UniformBufferHandleType transform_uniform_id_;
	VertexBufferHandleType vbuffer_id_;
	IndexBufferHandleType ibuffer_id_;
	bool enabled_;
};

}

#endif
