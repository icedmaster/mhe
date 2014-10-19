#include "render/gbuffer_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/uniforms.hpp"
#include "render/render_context.hpp"

namespace mhe {

bool ClearCommand::execute()
{
	ASSERT(driver_ != nullptr, "You must to setup the driver first");
	driver_->clear(true, true, true, vec4(0.0, 0.0, 0.0, 0.0));
	return true;
}

GBufferFillMaterialSystem::GBufferFillMaterialSystem()
{
	gbuffer_desc_.target = rt_readwrite;
	gbuffer_desc_.width = gbuffer_desc_.height = 0;
	gbuffer_desc_.color_datatype[0] = format_ubyte;
	gbuffer_desc_.color_datatype[1] = format_float;
	gbuffer_desc_.color_format[0] = format_rgba;
	gbuffer_desc_.color_format[1] = format_rgba32f;
	gbuffer_desc_.color_targets = 2;
	gbuffer_desc_.depth_format = format_d24f;
	gbuffer_desc_.depth_datatype = format_float;
	gbuffer_desc_.use_depth = true;
	gbuffer_desc_.use_stencil = false;
}

bool GBufferFillMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	StandartGeometryLayout::init(context);
	set_layout(StandartGeometryLayout::handle);

	clear_command_.set_driver(&context.driver);

	if (!context.shader_manager.get(shader(), material_system_context.shader_name))
		return false;

	transform_uniform_ = context.uniform_pool.create();
	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	UniformBufferDesc uniform_buffer_desc;
	create_uniform_buffer_element(uniform_buffer_desc, "vp", mat4x4::identity());
	uniform_buffer_desc.name = "transform";
	uniform_buffer_desc.program = &default_program(context);
	if (!uniform.init(uniform_buffer_desc))
		return false;

	render_target_ = context.render_target_pool.create();
	RenderTarget& render_target = context.render_target_pool.get(render_target_);
	if (!gbuffer_desc_.width) gbuffer_desc_.width = context.window_system.width();
	if (!gbuffer_desc_.height) gbuffer_desc_.height = context.window_system.height();
	return render_target.init(context, gbuffer_desc_);
}

void GBufferFillMaterialSystem::close()
{}

void GBufferFillMaterialSystem::setup(Context& context, Node* nodes, ModelContext* model_contexts, size_t count)
{
	standart_material_setup(context, nodes, model_contexts, count, 1);
	Material* materials = context.materials[id()].all_objects();
	for (size_t i = 0; i < count; ++i)
	{
		DrawCallData& draw_call_data = context.draw_call_data_pool.get(nodes[i].main_pass.draw_call_data);
		draw_call_data.render_target = render_target_ + 1;
		if (i == 0)
			draw_call_data.command = &clear_command_;
	}
}

void GBufferFillMaterialSystem::destroy(Context& context, Node* nodes, size_t count)
{
}

void GBufferFillMaterialSystem::update(Context& context, RenderContext& render_context, Node* nodes, Transform* transforms, size_t* transform_indices, size_t count)
{
	TransformSimpleData transform_data;
	transform_data.vp = render_context.vp;
	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	uniform.update(transform_data);
}

void GBufferFillMaterialSystem::setup_uniforms(Material& material, Context& context, const ModelContext& model_context)
{
	material.uniforms[0] = transform_uniform_;
	UniformBuffer::IdType id = context.uniform_pool.create();
	material.uniforms[1] = id;
	UniformBuffer& uniform = context.uniform_pool.get(id);
	UniformBufferDesc uniform_buffer_desc;
	create_uniform_buffer_element(uniform_buffer_desc, "model", model_context.model);
	create_uniform_buffer_element(uniform_buffer_desc, "normal", model_context.model);
	uniform_buffer_desc.name = "permodel";
	uniform_buffer_desc.program = &default_program(context);
	uniform.init(uniform_buffer_desc);
}

bool GBufferDrawMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	FullscreenLayout::init(context);

	RenderTargetDesc light_buffer_desc;
	light_buffer_desc.target = rt_readwrite;
	light_buffer_desc.color_datatype[0] = format_float;
	light_buffer_desc.color_format[0] = format_rgba32f;
	light_buffer_desc.color_targets = 1;
	light_buffer_desc.use_depth = false;
	light_buffer_desc.use_stencil = false;
	light_buffer_desc.width = context.window_system.width();
	light_buffer_desc.height = context.window_system.height();
	light_buffer_render_target_ = context.render_target_pool.create();
	RenderTarget& light_render_target = context.render_target_pool.get(light_buffer_render_target_);
	if (!light_render_target.init(context, light_buffer_desc))
		return false;

	const TextureInstance* light_texture;
	light_render_target.color_textures(&light_texture);
	light_buffer_texture_ = light_texture[0];

	const std::string& lights_number = material_system_context.defs[0];
	if (!lights_number.empty())
		lights_per_pass_ = types_cast<size_t>(lights_number);
	else lights_per_pass_ = 1;
	if (!context.shader_manager.get(shader(), material_system_context.shader_name))
		return false;

	if (!lights_number.empty())
	{
		for (size_t i = 0; i < max_lights_number; ++i)
		{
			light_uniform_[i] = context.uniform_pool.create();
			UniformBuffer& uniform = context.uniform_pool.get(light_uniform_[i]);
			UniformBufferDesc uniform_buffer_desc;
			uniform_buffer_desc.name = "lights";
			uniform_buffer_desc.program = &default_program(context);
			if (!uniform.init(uniform_buffer_desc))
				return false;
		}
	}

	transform_uniform_ = context.uniform_pool.create();
	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	UniformBufferDesc uniform_buffer_desc;
	uniform_buffer_desc.name = "transform";
	uniform_buffer_desc.program = &default_program(context);
	if (!uniform.init(uniform_buffer_desc))
		return false;

	clear_command_.set_driver(&context.driver);

	return true;
}

void GBufferDrawMaterialSystem::close()
{
}

void GBufferDrawMaterialSystem::setup(Context& context, Node* nodes, ModelContext* /*model_contexts*/, size_t count)
{
	ASSERT(count == 1, "We draw the only one posteffect");
	Node& node = nodes[0];

	RenderTarget& render_target = context.render_target_pool.get(render_target_);
	const TextureInstance* textures = nullptr;
	size_t number = render_target.color_textures(&textures);
	ASSERT(number != 0, "Invalid render target");
	TextureInstance depth_texture;
	number = render_target.depth_texture(depth_texture);
	ASSERT(number != 0, "Invalid depth texture");
	
	Material& material = create_and_get(context.materials[id()]);
	material.shader_program = ubershader(context).get_default();
	material.textures[0] = textures[1];
	material.textures[1] = depth_texture;
	material.uniforms[0] = transform_uniform_;
	material.uniforms[1] = light_uniform_[0];
	
	node.main_pass.material.material_system = id();
	node.main_pass.material.id = material.id;

	node.mesh.layout = FullscreenLayout::handle;

	node.main_pass.draw_call_data = context.draw_call_data_pool.create();
	DrawCallData& draw_call_data = context.draw_call_data_pool.get(node.main_pass.draw_call_data);
	draw_call_data.render_target = light_buffer_render_target_ + 1;
	draw_call_data.command = &clear_command_;
	draw_call_data.state = context.render_state_pool.create();
	RenderState& render_state = context.render_state_pool.get(draw_call_data.state);
	RenderStateDesc desc;
	desc.depth.enabled = false;
	desc.blend.enabled = true;
	desc.blend.dstmode = blend_one;
	desc.blend.srcmode = blend_one;
	render_state.init(desc);

	node.additional_passes = context.additional_passes_pool.create();
}

void GBufferDrawMaterialSystem::destroy(Context& context, Node* nodes, size_t count)
{}

void GBufferDrawMaterialSystem::update(Context& context, RenderContext& render_context, Node* nodes, Transform* /*transforms*/, size_t* /*transform_indices*/, size_t count)
{
	ASSERT(count == 1 && nodes != nullptr, "Invalid nodes");

	TransformData transform_data;
	transform_data.vp = render_context.vp;
	transform_data.inv_proj = render_context.proj.inverted();
	transform_data.inv_vp = render_context.inv_vp;
	transform_data.viewpos = render_context.viewpos;
	context.uniform_pool.get(transform_uniform_).update(transform_data);

	Material& main_material = context.materials[id()].get(nodes[0].main_pass.material.id);
	DrawCallData& main_draw_call_data = context.draw_call_data_pool.get(nodes[0].main_pass.draw_call_data);

	size_t lights_per_every_pass[max_additional_render_passes_number];
	::memset(lights_per_every_pass, 0, sizeof(lights_per_every_pass));
	size_t passes_number = calculate_passes_number(render_context, lights_per_every_pass);
	if (passes_number == 0) return;

	AdditionalPasses& passes = context.additional_passes_pool.get(nodes[0].additional_passes);
	passes.passes.resize(passes_number - 1);
	LightData data[max_lights_number];
	size_t i = 0;
	for (size_t pass_number = 0; pass_number < passes_number; ++pass_number)
	{
		int type = 0;
		for (size_t l = 0; l < lights_per_every_pass[pass_number]; ++l, ++i)
		{
			const Light& light = render_context.lights[i];
			data[l].diffuse = light.shading().diffuse;
			data[l].diffuse.set_w(light.type() == Light::spot ? light.spot_angle_coeff() : light.angle());
			data[l].specular = light.shading().specular;
			data[l].position = vec4(light.position().x(), light.position().y(), light.position().z(), light.attenuation());
			data[l].direction = vec4(light.direction().x(), light.direction().y(), light.direction().z(), light.angle_attenuation());
			type = light.type();
		}

		UniformBuffer& uniform = context.uniform_pool.get(light_uniform_[pass_number]);
		uniform.update(reinterpret_cast<const uint8_t*>(&data[0]), sizeof(LightData) * lights_per_every_pass[pass_number]);

		UberShader& shader = ubershader(context);
		UberShader::Index ubershader_index;
		ubershader_index.set(shader.info("LIGHT_TYPE"), type);
		ubershader_index.set(shader.info("LIGHTS_NUMBER"), lights_per_every_pass[pass_number]);
		ShaderProgram::IdType shader_program_id = ubershader(context).get(ubershader_index);

		if (pass_number == 0)
			main_material.shader_program = shader_program_id;

		if (pass_number > 0)
		{
			RenderPassData& pass = passes.passes[pass_number - 1];
			if (!context.materials[id()].is_valid(pass.material))
				pass.material = context.materials[id()].create();
			Material& material = context.materials[id()].get(pass.material);
			material.shader_program = shader_program_id;
			for (size_t j = 0; j < material_textures_number; ++j)
				material.textures[j] = main_material.textures[j];
			material.uniforms[0] = transform_uniform_;
			material.uniforms[1] = light_uniform_[pass_number];

			if (!context.draw_call_data_pool.is_valid(pass.draw_call_data))
				pass.draw_call_data = context.draw_call_data_pool.create();
			DrawCallData& draw_call_data = context.draw_call_data_pool.get(pass.draw_call_data);
			draw_call_data.command = nullptr;
			draw_call_data.render_target = main_draw_call_data.render_target;
			draw_call_data.state = main_draw_call_data.state;
		}
	}
}

size_t GBufferDrawMaterialSystem::calculate_passes_number(RenderContext& render_context, size_t* passes) const
{
	size_t indexes[Light::light_types_number];
	::memset(indexes, 0, sizeof(indexes));
	for (size_t i = 0; i < render_context.lights_number; ++i)
	{
		const Light& light = render_context.lights[i];
		++indexes[light.type()];
	}

	size_t idx = 0;
	size_t number = 0;
	for (size_t i = 0; i < Light::light_types_number; ++i)
	{
		size_t passes_per_type = indexes[i] / lights_per_pass_ + (indexes[i] % lights_per_pass_ == 0 ? 0 : 1);
		if (!passes_per_type) continue;
		size_t j = 0;
		for (size_t end = (passes_per_type > 1) ? passes_per_type - 1 : 0; j < end; ++j)
			passes[idx++] = lights_per_pass_;
		passes[idx++] = indexes[i] - lights_per_pass_ * j;
		number += passes_per_type;
	}
	return number;
}

}
