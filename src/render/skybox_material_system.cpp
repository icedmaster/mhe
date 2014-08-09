#include "render/skybox_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/render_context.hpp"
#include "render/node.hpp"

namespace mhe {

bool SkyboxMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	SkyboxLayout::init(context);

	if (!context.shader_manager.get(shader_, material_system_context.shader_name))
		return false;
	transform_uniform_ = context.uniform_pool.create();
	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	UniformBufferDesc uniform_buffer_desc;
	create_uniform_buffer_element(uniform_buffer_desc, "inv_vp", mat4x4::identity());
	uniform_buffer_desc.name = "transform";
	uniform_buffer_desc.program = &context.shader_pool.get(shader_.shader_program_handle);
	return uniform.init(uniform_buffer_desc);
}

void SkyboxMaterialSystem::close()
{
}

void SkyboxMaterialSystem::setup(Context& context, Node* nodes, ModelContext* model_contexts, size_t count)
{
    DrawCallData& draw_call_data = create_and_get(context.draw_call_data_pool);
    draw_call_data.state = context.render_state_pool.create();
    RenderState& state = context.render_state_pool.get(draw_call_data.state);
    RenderStateDesc desc;
    desc.depth.enabled = false;
    state.init(desc);
	for (size_t i = 0; i < count; ++i)
	{
		nodes[i].material.material_system = id();
		nodes[i].material.id = context.materials[id()].create();
		Material& material = context.materials[id()].get(nodes[i].material.id);
		material.shader_program = shader_.shader_program_handle;
		material.uniforms[0] = transform_uniform_;
		context.texture_manager.get(material.textures[0], model_contexts[i].textures[0]);
		nodes[i].mesh.layout = SkyboxLayout::handle;
        nodes[i].draw_call_data = draw_call_data.id;
	}
}

void SkyboxMaterialSystem::destroy(Context& context, Node* nodes, size_t count)
{
}

void SkyboxMaterialSystem::update(Context& context, RenderContext& render_context,
	Node* /*nodes*/, Transform* /*transforms*/, size_t* /*indexes*/,size_t /*count*/)
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

size_t SkyboxMaterialSystem::layout() const
{
	return SkyboxLayout::handle;
}

}
