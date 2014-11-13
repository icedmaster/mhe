#include "render/skybox_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/render_context.hpp"
#include "render/instances.hpp"

namespace mhe {

bool SkyboxMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	SkyboxLayout::init(context);
	set_layout(SkyboxLayout::handle);

	if (!context.shader_manager.get(shader(), material_system_context.shader_name))
		return false;
	transform_uniform_ = context.uniform_pool.create();
	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	UniformBufferDesc uniform_buffer_desc;
	create_uniform_buffer_element(uniform_buffer_desc, "inv_vp", mat4x4::identity());
	uniform_buffer_desc.name = "transform";
	uniform_buffer_desc.program = &default_program(context);
	return uniform.init(uniform_buffer_desc);
}

void SkyboxMaterialSystem::close()
{
}

void SkyboxMaterialSystem::setup(Context& context, SceneContext& scene_context, NodeInstance* nodes, ModelContext* model_contexts, size_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		nodes[i].node.main_pass.material.material_system = id();
		nodes[i].node.main_pass.material.id = context.materials[id()].create();
		Material& material = context.materials[id()].get(nodes[i].node.main_pass.material.id);
		material.shader_program = ubershader(context).get_default();
		material.uniforms[0] = transform_uniform_;
		context.texture_manager.get(material.textures[0], model_contexts[i].textures[0]);
		nodes[i].node.mesh.layout = SkyboxLayout::handle;

		RenderStateDesc render_state_desc;
		render_state_desc.stencil.enabled = false;
		render_state_desc.depth.enabled = false;
		nodes[i].node.main_pass.draw_call_data = context.draw_call_data_pool.create();
		DrawCallData& draw_call_data = context.draw_call_data_pool.get(nodes[i].node.main_pass.draw_call_data);
		draw_call_data.state = context.render_state_pool.create();
		context.render_state_pool.get(draw_call_data.state).init(render_state_desc);

		context.additional_passes_pool.make_invalid(nodes[i].node.additional_passes);
	}
}

void SkyboxMaterialSystem::destroy(Context& context, SceneContext& scene_context, NodeInstance* nodes, size_t count)
{
}

void SkyboxMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context,
	NodeInstance* /*nodes*/, size_t /*count*/)
{
	mat4x4 inv_vp = render_context.vp;
	inv_vp.set_row(3, vec4(0, 0, 0, 1));
	inv_vp.inverse();

	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	UniformBufferDesc uniform_buffer_desc;
	create_uniform_buffer_element(uniform_buffer_desc, "inv_vp", inv_vp);
	uniform_buffer_desc.name = "transform";
	uniform.update(uniform_buffer_desc);
}

}