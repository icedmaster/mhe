#include "render/posteffect_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/instances.hpp"
#include "render/utils/simple_meshes.hpp"
#include "render/render_context.hpp"

namespace mhe {

bool PosteffectSimpleMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	set_layout(FullscreenLayout::handle);

    if (!context.shader_manager.get(shader(), material_system_context.shader_name))
    {
        ERROR_LOG("Can't get shader:" << material_system_context.shader_name);
        return false;
    }

    return init_mesh(context, material_system_context);
}

bool PosteffectSimpleMaterialSystem::init_mesh(Context& context, const MaterialSystemContext& /*material_system_context*/)
{
	DrawCallData& draw_call_data = create_and_get(context.draw_call_data_pool);
	RenderState& render_state = create_and_get(context.render_state_pool);

	draw_call_data.state = render_state.id();
	RenderStateDesc desc;
	desc.blend.enabled = true;
	desc.blend.srcmode = blend_src_alpha;
	desc.blend.dstmode = blend_src_inv_alpha;
	desc.depth.enabled = false;
	render_state.init(desc);

	mesh_.mesh.parts.resize(1);
	mesh_.instance_parts.resize(1);
	if (!utils::create_fullscreen_quad(mesh_, context))
	{
		ERROR_LOG("Can't create fullscreen quad");
		return false;
	}
	mesh_.mesh.parts[0].render_data.layout = layout();

	mesh_.instance_parts[0].draw_call_data = draw_call_data.id;

	Material& material = create_and_get(context.materials[id()]);
	material.shader_program = default_program(context).id();
	mesh_.instance_parts[0].material.material_system = id();
	mesh_.instance_parts[0].material.id = material.id;
	
	return true;
}

void PosteffectSimpleMaterialSystem::close()
{
}

void PosteffectSimpleMaterialSystem::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void PosteffectSimpleMaterialSystem::update(Context& context, SceneContext& /*scene_context*/, RenderContext& render_context)
{
	Material& material = context.materials[id()].get(mesh_.instance_parts[0].material.id);
	for (size_t j = 0; j < max_textures_number; ++j)
		material.textures[j] = textures_[j];
	material.uniforms[0] = render_context.main_camera.percamera_uniform;
	setup_draw_call(render_context.draw_calls.add(), mesh_.instance_parts[0], mesh_.mesh.parts[0]);
}

bool PosteffectDebugMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	texture_type_mask_ = 0;
	set_layout(FullscreenLayout::handle);

	if (!context.shader_manager.get(shader(), material_system_context.shader_name))
	{
		ERROR_LOG("Can't get shader:" << material_system_context.shader_name);
		return false;
	}

	return init_mesh(context, material_system_context);
}

bool PosteffectDebugMaterialSystem::init_mesh(Context& context, const MaterialSystemContext& material_system_context)
{
	const vec2& viewport = material_system_context.options.get<vec2>("viewport");
	bool overdraw = material_system_context.options.get<bool>("overdraw");

	int window_width = context.window_system.width();
	int window_height = context.window_system.height();
	rect<int> viewports[max_textures_number];
	if (viewport.x() == 0.0f && viewport.y() == 0.0f)
	{
		viewports[0] = rect<int>(0, window_height / 2, window_width / 2, window_height / 2);
		viewports[1] = rect<int>(window_width / 2, window_height / 2, window_width / 2, window_height / 2);
		viewports[2] = rect<int>(0, 0, window_width / 2, window_height / 2);
		viewports[3] = rect<int>(window_width / 2, 0, window_width / 2, window_height / 2);
	}

	for (size_t i = 0; i < max_textures_number; ++i)
	{
		DrawCallData& draw_call_data = create_and_get(context.draw_call_data_pool);
		RenderState& render_state = create_and_get(context.render_state_pool);

		draw_call_data.state = render_state.id();
		RenderStateDesc desc;
		desc.blend.enabled = true;
		desc.viewport.viewport = viewports[i];
		if (!overdraw)
		{
			desc.blend.srcmode = blend_src_alpha;
			desc.blend.dstmode = blend_src_inv_alpha;
		}
		else
		{
			desc.blend.srcmode = blend_one;
			desc.blend.dstmode = blend_zero;
		}
		desc.depth.enabled = false;
		render_state.init(desc);

		mesh_[i].mesh.parts.resize(1);
		mesh_[i].instance_parts.resize(1);
		if (!utils::create_fullscreen_quad(mesh_[i], context))
		{
			ERROR_LOG("Can't create fullscreen quad");
			return false;
		}
		mesh_[i].mesh.parts[0].render_data.layout = layout();

		mesh_[i].instance_parts[0].draw_call_data = draw_call_data.id;

		Material& material = create_and_get(context.materials[id()]);
		material.shader_program = default_program(context).id();
		mesh_[i].instance_parts[0].material.material_system = id();
		mesh_[i].instance_parts[0].material.id = material.id;
	}

	return true;
}

void PosteffectDebugMaterialSystem::close()
{
}

void PosteffectDebugMaterialSystem::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
	empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void PosteffectDebugMaterialSystem::update(Context& context, SceneContext& /*scene_context*/, RenderContext& render_context)
{
	UberShader& shader = ubershader(context);
	UberShader::Index ubershader_index;
	const UberShader::Info& info = shader.info("RENDER_DEPTH");
	ubershader_index.set(info, 1);

	for (size_t j = 0; j < max_textures_number; ++j)
	{
		if (textures_[j].id == Texture::invalid_id) continue;
		Material& material = context.materials[id()].get(mesh_[j].instance_parts[0].material.id);
		material.textures[0] = textures_[j];

		if (texture_type_mask_ & (1 << j))
			material.shader_program = shader.get(ubershader_index);

		setup_draw_call(render_context.draw_calls.add(), mesh_[j].instance_parts[0], mesh_[j].mesh.parts[0]);
	}
}

void PosteffectDebugMaterialSystem::set_render_target(const RenderTarget& render_target)
{
	size_t color_textures_number = render_target.color_textures(textures_);
	size_t depth_textures_number = render_target.depth_texture(textures_[max_textures_number - 1]);
	texture_type_mask_ = depth_textures_number == 0 ? 0 : (1 << 3);
}

}
