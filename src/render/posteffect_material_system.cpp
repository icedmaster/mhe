#include "render/posteffect_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/instances.hpp"
#include "render/utils/simple_meshes.hpp"
#include "render/render_context.hpp"
#include "utils/strutils.hpp"

namespace mhe {

bool PosteffectDebugMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	texture_type_mask_ = 0;
	textures_number_ = 0;
	set_layout(FullscreenLayout::handle);

	if (!context.shader_manager.get(shader(), material_system_context.shader_name))
	{
		ERROR_LOG("Can't get shader:" << material_system_context.shader_name);
		return false;
	}

	clear_command_.set_driver(&context.driver);

	return init_mesh(context, material_system_context);
}

bool PosteffectDebugMaterialSystem::init_mesh(Context& context, const MaterialSystemContext& material_system_context)
{
	bool overdraw = material_system_context.options.get<bool>("overdraw");

	int window_width = context.window_system.width();
	int window_height = context.window_system.height();
	rect<int> viewports[max_textures_number];
	viewports[0] = rect<int>(0, window_height / 2, window_width / 2, window_height / 2);
	viewports[1] = rect<int>(window_width / 2, window_height / 2, window_width / 2, window_height / 2);
	viewports[2] = rect<int>(0, 0, window_width / 2, window_height / 2);
	viewports[3] = rect<int>(window_width / 2, 0, window_width / 2, window_height / 2);

	for (size_t i = 0; i < max_textures_number; ++i)
	{
		if (!init_mesh_instance(context, mesh_[i], viewports[i], overdraw))
		{
			ERROR_LOG("Can't initialize mesh for PosteffectDebug");
			return false;
		}
	}

	if (!init_mesh_instance(context, fullscreen_mesh_, rect<int>(0, 0, window_width, window_height), overdraw))
	{
		ERROR_LOG("Can't initialize mesh for PosteffectDebug");
		return false;
	}

	return true;
}

bool PosteffectDebugMaterialSystem::init_mesh_instance(Context& context, MeshInstance& mesh_instance, const rect<int>& viewport, bool overdraw)
{
	DrawCallData& draw_call_data = create_and_get(context.draw_call_data_pool);
	RenderState& render_state = create_and_get(context.render_state_pool);

	draw_call_data.state = render_state.id();
	RenderStateDesc desc;
	desc.blend.enabled = true;
	desc.viewport.viewport = viewport;
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

	mesh_instance.mesh.parts.resize(1);
	mesh_instance.instance_parts.resize(1);
	if (!utils::create_fullscreen_quad(mesh_instance, context))
	{
		ERROR_LOG("Can't create fullscreen quad");
		return false;
	}
	mesh_instance.mesh.parts[0].render_data.layout = layout();

	mesh_instance.instance_parts[0].draw_call_data = draw_call_data.id;

	Material& material = create_and_get(context.materials[id()]);
	material.shader_program = default_program(context).id();
	mesh_instance.instance_parts[0].material.material_system = id();
	mesh_instance.instance_parts[0].material.id = material.id;

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

	clear_command_.reset();

	if (textures_number_ == 1) // can draw fullscreen
	{
		Material& material = context.materials[id()].get(fullscreen_mesh_.instance_parts[0].material.id);
		size_t texture_index = textures_[0].id == Texture::invalid_id ? max_textures_number - 1 : 0;
		material.textures[0] = textures_[texture_index];
		if (texture_type_mask_ & (1 << texture_index))
			material.shader_program = shader.get(ubershader_index);
		else material.shader_program = shader.get_default();
		setup_draw_call(render_context.draw_calls.add(), fullscreen_mesh_.instance_parts[0], fullscreen_mesh_.mesh.parts[0], &clear_command_);
	}
	else
	{
		for (size_t j = 0; j < max_textures_number; ++j)
		{
			if (textures_[j].id == Texture::invalid_id) continue;
			Material& material = context.materials[id()].get(mesh_[j].instance_parts[0].material.id);
			material.textures[0] = textures_[j];

			if (texture_type_mask_ & (1 << j))
				material.shader_program = shader.get(ubershader_index);

			setup_draw_call(render_context.draw_calls.add(), mesh_[j].instance_parts[0], mesh_[j].mesh.parts[0], &clear_command_);
		}
	}
}

void PosteffectDebugMaterialSystem::set_render_target(const RenderTarget& render_target)
{
	for (size_t i = 0; i < max_textures_number; ++i)
		textures_[i].id = Texture::invalid_id;
	size_t color_textures_number = render_target.color_textures(textures_);
	size_t depth_textures_number = render_target.depth_texture(textures_[max_textures_number - 1]);
	texture_type_mask_ = depth_textures_number == 0 ? 0 : (1 << 3);
	textures_number_ = color_textures_number + depth_textures_number;
}

PosteffectMaterialSystemBase::PosteffectMaterialSystemBase() :
	inputs_number_(0), outputs_number_(0), framebuffer_input_(invalid_index)
{}

bool PosteffectMaterialSystemBase::init(Context& context, const MaterialSystemContext& material_system_context)
{
	set_layout(FullscreenLayout::handle);

	if (!context.shader_manager.get(shader(), material_system_context.shader_name))
	{
		ERROR_LOG("Can't get shader:" << material_system_context.shader_name);
		return false;
	}

	clear_command_.set_driver(&context.driver);

	inputs_number_shader_info_ = ubershader(context).info("INPUTS");

	return init_mesh(context, material_system_context);
}

void PosteffectMaterialSystemBase::close()
{}

void PosteffectMaterialSystemBase::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
	empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void PosteffectMaterialSystemBase::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
	list_of_commands_.clear();
	clear_command_.reset();
	if (framebuffer_input_ != invalid_index) // current framebuffer as input
	{
		copy_framebuffer_command_.set_texture(&context.texture_pool.get(inputs_[framebuffer_input_].id));
		list_of_commands_.add_command(&copy_framebuffer_command_);
	}

	DrawCallData& draw_call_data = context.draw_call_data_pool.get(mesh_.instance_parts[0].draw_call_data);
	if (draw_call_data.render_target != default_render_target &&
		draw_call_data.render_target != RenderTarget::invalid_id)
		list_of_commands_.add_command(&clear_command_);
		
	RenderCommand* command = list_of_commands_.empty() ? nullptr : &list_of_commands_;

	UberShader::Index ubershader_index;
	ubershader_index.set(inputs_number_shader_info_, inputs_number_ - 1);
	UberShader& shader = ubershader(context);
	
	Material& material = context.materials[id()].get(mesh_.instance_parts[0].material.id);
	for (size_t j = 0; j < max_textures_number; ++j)
		material.textures[j] = inputs_[j];
	material.uniforms[0] = render_context.main_camera.percamera_uniform;
	material.shader_program = shader.get(ubershader_index);
	setup_draw_call(render_context.draw_calls.add(), mesh_.instance_parts[0], mesh_.mesh.parts[0], command);
}

bool PosteffectMaterialSystemBase::init_mesh(Context& context, const MaterialSystemContext& material_system_context)
{
	DrawCallData& draw_call_data = create_and_get(context.draw_call_data_pool);
	RenderState& render_state = create_and_get(context.render_state_pool);

	bool use_blend = material_system_context.options.get<bool>(string("blend"));

	draw_call_data.state = render_state.id();
	RenderStateDesc desc;
	desc.blend.enabled = use_blend;
	desc.blend.srcmode = blend_src_alpha;
	desc.blend.dstmode = blend_src_inv_alpha;
	desc.depth.enabled = false;
	render_state.init(desc);

	// outputs
	// TODO: add support for render target with multiply outputs
	const string& outputs_str = material_system_context.options.get<string>(string("outputs"));
	const std::vector<string>& outputs_scale = utils::split(outputs_str, string(","));
	if (!outputs_scale.empty())
	{
		create_output(draw_call_data, context, 0, types_cast<float>(outputs_scale[0]));
	}

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

bool PosteffectMaterialSystemBase::init_screen_input(Context& context, size_t index, uint8_t render_stage)
{
	ASSERT(input(index).id == Texture::invalid_id, "Invalid posteffects description");
	TextureDesc desc;
	desc.datatype = format_ubyte;
	desc.format = format_rgba;
	inputs_[index] = context.render_target_manager.create(context, desc, 1.0f);
	framebuffer_input_ = index;
	copy_framebuffer_command_.set_stages(render_stage);
	return true;
}

bool PosteffectMaterialSystemBase::create_output(Context& context, size_t index, float scale)
{
	ASSERT(output(index).id == Texture::invalid_id, "create_output() - trying to create a new output when old output still exists");
	DrawCallData& draw_call_data = context.draw_call_data_pool.get(mesh_.instance_parts[0].draw_call_data);
	ASSERT(draw_call_data.render_target == default_render_target || draw_call_data.render_target == RenderTarget::invalid_id,
		"RenderTarget has been set already");
	return create_output(draw_call_data, context, index, scale);
}

void PosteffectMaterialSystemBase::fill_render_target_desc(RenderTargetDesc& desc) const
{
	desc.target = rt_readwrite;
	desc.use_depth = false;
	desc.use_stencil = false;
	desc.color_targets = 1;
	desc.color_datatype[0] = format_ubyte;
	desc.color_format[0] = format_rgba;
}

bool PosteffectMaterialSystemBase::create_output(DrawCallData& draw_call_data, Context& context, size_t index, float scale)
{
	RenderTargetDesc render_target_desc;
	fill_render_target_desc(render_target_desc);
	RenderTarget& render_target = context.render_target_manager.create(context, render_target_desc, scale);
	draw_call_data.render_target = render_target.id();
	const TextureInstance* ids = nullptr;
	render_target.color_textures(&ids);
	set_output(index, ids[0]);

	if (scale != 1.0f)
	{
		RenderState& render_state = context.render_state_pool.get(draw_call_data.state);
		ViewportDesc desc;
		desc.viewport.set(0, 0, render_target_desc.width, render_target_desc.height);
		render_state.update_viewport(desc);
	}

	return true;
}

bool BlurMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	if (!PosteffectMaterialSystemBase::init(context, material_system_context))
		return false;
	pass_info_ = ubershader(context).info("PASS");

	MeshInstance& first_pass_mesh = mesh_instance();
	DrawCallData& first_pass_draw_call_data = context.draw_call_data_pool.get(first_pass_mesh.instance_parts[0].draw_call_data);
	second_pass_mesh_ = first_pass_mesh;
	DrawCallData& second_pass_draw_call_data = create_and_get(context.draw_call_data_pool);
	second_pass_draw_call_data = first_pass_draw_call_data;
	second_pass_mesh_.instance_parts[0].draw_call_data = second_pass_draw_call_data.id;

	Material& material = create_and_get(context.materials[id()]);
	second_pass_mesh_.instance_parts[0].material.id = material.id;

	clear_command_.set_driver(&context.driver);

	return true;
}

bool BlurMaterialSystem::create_output(DrawCallData& draw_call_data, Context& context, size_t index, float scale)
{
	if (!PosteffectMaterialSystemBase::create_output(draw_call_data, context, index, scale))
		return false;

	DrawCallData& second_pass_draw_call_data = context.draw_call_data_pool.get(second_pass_mesh_.instance_parts[0].draw_call_data);

	RenderTargetDesc render_target_desc;
	fill_render_target_desc(render_target_desc);
	RenderTarget& render_target = context.render_target_manager.create(context, render_target_desc, scale);
	second_pass_draw_call_data.render_target = render_target.id();

	// current output (first pass) as input for second pass
	Material& second_pass_material = context.materials[id()].get(second_pass_mesh_.instance_parts[0].material.id);
	second_pass_material.textures[0] = output(0);

	TextureInstance texture;
	render_target.color_texture(texture, 0);
	set_output(0, texture);

	return true;
}

void BlurMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
	PosteffectMaterialSystemBase::update(context, scene_context, render_context);

	Material& material = context.materials[id()].get(second_pass_mesh_.instance_parts[0].material.id);
	UberShader::Index ubershader_index;
	ubershader_index.set(pass_info_, 1);
	material.shader_program = ubershader(context).get(ubershader_index);
	material.uniforms[0] = render_context.main_camera.percamera_uniform;

	clear_command_.reset();
	setup_draw_call(render_context.draw_calls.add(), second_pass_mesh_.instance_parts[0], second_pass_mesh_.mesh.parts[0], &clear_command_);
}

}
