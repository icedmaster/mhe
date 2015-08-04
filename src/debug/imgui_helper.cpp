#include "debug/imgui_helper.hpp"

#include "game/engine.hpp"
#include "render/context.hpp"
#include "render/uniforms.hpp"
#include "events/event_manager.hpp"
#include "events/mouse_event.hpp"
#include "events/mouse_device.hpp"

// For ImGUI we have to invert mouse y coordinate and slightly change a projection matrix (0, w, h, 0 instead of 0, w, 0, h)
// TODO: need to implement ScissorTest

namespace mhe {

namespace {

vec4 to_vec4(uint32_t c)
{
	return vec4((c & 0xff) / 255.0f, ((c >> 8) & 0xff) / 255.0f, ((c >> 16) & 0xff) / 255.0f, ((c >> 24) & 0xff) / 255.0f);
}

void imgui_render(ImDrawList** const cmd_lists, int cmd_lists_count)
{
	size_t vertices = 0;
	DrawCallExplicit* draw_call = reinterpret_cast<DrawCallExplicit*>(ImGui::GetIO().UserData);
	ImGuiHelper::Vertex* vertex = reinterpret_cast<ImGuiHelper::Vertex*>(draw_call->vbuffer->map(10000 * sizeof(ImGuiHelper::Vertex), 0));
	for (int i = 0; i < cmd_lists_count; ++i)
	{
		for (size_t j = 0, size = cmd_lists[i]->vtx_buffer.size(); j < size; ++j, ++vertex)
		{
			vertex->pos = vec4(cmd_lists[i]->vtx_buffer[j].pos.x, cmd_lists[i]->vtx_buffer[j].pos.y, 0.0f, 1.0f);
			vertex->col = to_vec4(cmd_lists[i]->vtx_buffer[j].col);
			vertex->tex = vec2(cmd_lists[i]->vtx_buffer[j].uv.x, cmd_lists[i]->vtx_buffer[j].uv.y);
		}

		for (size_t j = 0, size = cmd_lists[i]->commands.size(); j < size; ++j)
			vertices += cmd_lists[i]->commands[j].vtx_count;
	}
	ASSERT(vertex != nullptr, "Invalid ptr");
	draw_call->vbuffer->unmap();

	draw_call->elements_number = vertices / 3;
	draw_call->indices_number = vertices;
	draw_call->ibuffer_offset = 0;
	draw_call->vbuffer_offset = 0;
}

}

bool ImGuiHelper::init(game::Engine* engine)
{
	enabled_ = true;

	Context& context = engine->context();
	Shader shader;
	if (!context.shader_manager.get(shader, string("imgui")))
	{
		ERROR_LOG("Can't initialize imgui shader");
		return false;
	}
	UberShader& ubershader = context.ubershader_pool.get(shader.shader_program_handle);
	shader_program_id_ = ubershader.get_default();

	if (!init_layout(context))
	{
		ERROR_LOG("Layout initialization failed");
		return false;
	}

	if (!init_render_state(context))
	{
		ERROR_LOG("RenderState initialization failed");
		return false;
	}

	if (!init_buffers(context))
	{
		ERROR_LOG("Buffers initialization failed");
		return false;
	}

	if (!init_uniform(context))
	{
		ERROR_LOG("Uniform initialization failed");
		return false;
	}

	return init_imgui(context);
}

void ImGuiHelper::destroy()
{
	ImGui::Shutdown();
}

void ImGuiHelper::update(Context& context, RenderContext& render_context, const EventManager& event_manager)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = render_context.fdelta;

	const vector2<int>& mouspos = event_manager.mouse()->position();
	io.MousePos = ImVec2(static_cast<float>(mouspos.x()), context.window_system.height() - static_cast<float>(mouspos.y()));
	io.MouseDown[0] = event_manager.mouse()->is_button_pressed(MouseEvent::left_button);
	io.MouseDown[1] = event_manager.mouse()->is_button_pressed(MouseEvent::right_button);

	ImGui::NewFrame();
}

void ImGuiHelper::render(Context& context, RenderContext& render_context)
{
	if (!enabled_) return;
	DrawCallExplicit& draw_call = render_context.explicit_draw_calls.add();
	prepare_draw_call(draw_call);
	draw_call.layout = &context.layout_pool.get(layout_id_);
	draw_call.primitive = triangle;
	draw_call.render_state = &context.render_state_pool.get(render_state_id_);
	draw_call.render_target = nullptr;
	draw_call.shader_program = &context.shader_pool.get(shader_program_id_);
	draw_call.ibuffer = &context.index_buffer_pool.get(ibuffer_id_);
	draw_call.vbuffer = &context.vertex_buffer_pool.get(vbuffer_id_);
	draw_call.textures[0] = &context.texture_pool.get(font_texture_id_);
	draw_call.uniforms[0] = &context.uniform_pool.get(transform_uniform_id_);
	draw_call.render_command = nullptr;

	ImGui::GetIO().UserData = &draw_call;
	ImGui::Render();
}

bool ImGuiHelper::init_layout(Context& context)
{
	LayoutDesc desc;
	desc.layout.resize(3);

	desc.layout[0].offset = 0;
	desc.layout[0].size = 4;
	desc.layout[0].stride = sizeof(Vertex);
	desc.layout[0].position = 0;

	desc.layout[1].offset = 4 * sizeof(float);
	desc.layout[1].size = 4;
	desc.layout[1].stride = sizeof(Vertex);
	desc.layout[1].position = 1;

	desc.layout[2].offset = 8 * sizeof(float);
	desc.layout[2].size = 2;
	desc.layout[2].stride = sizeof(Vertex);
	desc.layout[2].position = 2;

	Layout& layout = create_and_get(context.layout_pool);
	layout_id_ = layout.id();

	return layout.init(desc);
}

bool ImGuiHelper::init_render_state(Context& context)
{
	RenderStateDesc desc;
	desc.blend.enabled = true;
	desc.blend.srcmode = blend_src_alpha;
	desc.blend.dstmode = blend_src_inv_alpha;
	desc.blend.func = blend_add;
	desc.blend.func_alpha = blend_add;

	desc.depth.enabled = false;

	RenderState& render_state = create_and_get(context.render_state_pool);
	render_state_id_ = render_state.id();
	return render_state.init(desc);
}

bool ImGuiHelper::init_buffers(Context& context)
{
	VertexBuffer& vbuffer = create_and_get(context.vertex_buffer_pool);
	vbuffer_id_ = vbuffer.id();
	if (!vbuffer.init(buffer_update_type_dynamic, nullptr, sizeof(Vertex) * 10000, sizeof(Vertex)))
		return false;
	IndexBuffer& ibuffer = create_and_get(context.index_buffer_pool);
	ibuffer_id_ = ibuffer.id();
	// TODO: create a method for rendering without index buffer?
	uint32_t indices[10000];
	for (size_t i = 0; i < 10000; ++i) indices[i] = i;
	return ibuffer.init(buffer_update_type_dynamic, vbuffer, indices, 10000);
}

bool ImGuiHelper::init_uniform(Context& context)
{
	UniformBufferDesc desc;
	desc.unit = 0;
	desc.update_type = uniform_buffer_normal;
	desc.size = sizeof(TransformSimpleData);

	UniformBuffer& uniform = create_and_get(context.uniform_pool);
	transform_uniform_id_ = uniform.id();
	if (uniform.init(desc))
	{
		update_uniform(uniform, context);
		return true;
	}
	return false;
}

bool ImGuiHelper::init_imgui(Context& context)
{
	ImGuiIO& io = ImGui::GetIO();
	io.RenderDrawListsFn = imgui_render;
	io.DisplaySize.x = static_cast<float>(context.window_system.width());
	io.DisplaySize.y = static_cast<float>(context.window_system.height());

	uint8_t* pixels = nullptr;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	TextureDesc desc;
	desc.address_mode_s = texture_clamp;
	desc.address_mode_t = texture_clamp;
	desc.anisotropic_level = 0.0f;
	desc.mips = 0;
	desc.datatype = format_ubyte;
	desc.format = format_rgba;
	desc.height = height;
	desc.width = width;
	desc.type = texture_2d;

	Texture& texture = create_and_get(context.texture_pool);
	font_texture_id_ = texture.id();
	return texture.init(desc, pixels, width * height * 4);
}

void ImGuiHelper::update_uniform(UniformBuffer& uniform_buffer, Context& context)
{
	float w = static_cast<float>(context.window_system.width());
	float h = static_cast<float>(context.window_system.height());
	mat4x4 proj;
	proj.set_ortho(0, w, h, 0, 0.0f, 10.0f);
	TransformSimpleData transform;
	transform.vp = proj;
	uniform_buffer.update(transform);
}

}
