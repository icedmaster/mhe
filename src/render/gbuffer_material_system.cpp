#include "render/gbuffer_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/uniforms.hpp"
#include "render/render_context.hpp"

namespace mhe {

bool GBufferFillMaterialSystem::ClearCommand::execute()
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

	clear_command_.set_driver(&context.driver);

	if (!context.shader_manager.get(shader_, material_system_context.shader_name))
		return false;

	transform_uniform_ = context.uniform_pool.create();
	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	UniformBufferDesc uniform_buffer_desc;
	create_uniform_buffer_element(uniform_buffer_desc, "vp", mat4x4::identity());
	uniform_buffer_desc.name = "transform";
	uniform_buffer_desc.program = &context.shader_pool.get(shader_.shader_program_handle);
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
		DrawCallData& draw_call_data = context.draw_call_data_pool.get(nodes[i].draw_call_data);
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
	TransformData transform_data;
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
	uniform_buffer_desc.program = &context.shader_pool.get(shader_.shader_program_handle);
	uniform.init(uniform_buffer_desc);
}

size_t GBufferFillMaterialSystem::layout() const
{
	return StandartGeometryLayout::handle;
}

bool GBufferDrawMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	FullscreenLayout::init(context);

	return context.shader_manager.get(shader_, material_system_context.shader_name);
}

void GBufferDrawMaterialSystem::close()
{
}

void GBufferDrawMaterialSystem::setup(Context& context, Node* nodes, ModelContext* /*model_contexts*/, size_t count)
{
	ASSERT(count == 1, "We draw the only one posteffect");
	Node& node = nodes[0];

	RenderTarget& render_target = context.render_target_pool.get(render_target_);
	const Texture::IdType* textures = nullptr;
	size_t number = render_target.color_textures(&textures);
	ASSERT(number > 0, "Only color targets supported now");
	
	Material& material = create_and_get(context.materials[id()]);
	material.shader_program = shader_.shader_program_handle;
	material.textures[0].id = textures[0];
	
	node.material.material_system = id();
	node.material.id = material.id;

	node.mesh.layout = FullscreenLayout::handle;

	node.draw_call_data = context.draw_call_data_pool.create();
	DrawCallData& draw_call_data = context.draw_call_data_pool.get(node.draw_call_data);
	draw_call_data.state = context.render_state_pool.create();
	RenderState& render_state = context.render_state_pool.get(draw_call_data.state);
	RenderStateDesc desc;
	desc.depth.enabled = false;
	desc.blend.enabled = true;
	desc.blend.dstmode = blend_src_inv_alpha;
	desc.blend.srcmode = blend_src_alpha;
	render_state.init(desc);
}

void GBufferDrawMaterialSystem::destroy(Context& context, Node* nodes, size_t count)
{}

void GBufferDrawMaterialSystem::update(Context& /*context*/, RenderContext& /*render_context*/, Node* /*nodes*/, Transform* /*transforms*/, size_t* /*transform_indices*/, size_t /*count*/)
{
}

size_t GBufferDrawMaterialSystem::layout() const
{
	return FullscreenLayout::handle;
}

}
