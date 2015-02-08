#include "render/depth_write_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/render_context.hpp"
#include "render/scene_context.hpp"
#include "render/instances.hpp"
#include "render/light_instance_methods.hpp"

namespace mhe {

bool DepthWriteMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	clear_command_.set_driver(&context.driver);

	StandartGeometryLayout::init(context);
	set_layout(StandartGeometryLayout::handle);

	if (!init_default(context, material_system_context))
		return false;

	ShaderProgram& shader_program = default_program(context);

	UniformBuffer& uniform_buffer = create_and_get(context.uniform_pool);
	UniformBufferDesc desc;
	desc.name = "transform";
	desc.program = &shader_program;
	if (!uniform_buffer.init(desc))
		return false;
	uniform_ = uniform_buffer.id();

	return init_draw_call_data(context);
}

bool DepthWriteMaterialSystem::init_draw_call_data(Context& context)
{
	DrawCallData& draw_call_data = create_and_get(context.draw_call_data_pool);
	RenderState& render_state = create_and_get(context.render_state_pool);
	RenderStateDesc render_state_desc;
	render_state_desc.depth.enabled = true;
	if (!render_state.init(render_state_desc)) return false;

	RenderTarget& render_target = create_and_get(context.render_target_pool);
	RenderTargetDesc desc;
	desc.target = rt_readwrite;
	desc.color_targets = 0;
	desc.use_depth = true;
	desc.use_stencil = false;
	desc.depth_datatype = format_float;
	desc.depth_format = format_d24f;
	desc.width = context.window_system.width();
	desc.height = context.window_system.height();
	if (!render_target.init(context, desc)) return false;

	render_target.depth_texture(shadowmap_);

	draw_call_data.render_target = render_target.id();
	draw_call_data.state = render_state.id();

	draw_call_data_ = draw_call_data.id;
	return true;
}

void DepthWriteMaterialSystem::close()
{
}

void DepthWriteMaterialSystem::setup(Context& /*context*/, SceneContext& /*scene_context*/, MeshPartInstance* /*parts*/, MeshPart* /*part*/, ModelContext* /*model_contexts*/, size_t /*count*/)
{
	ASSERT(0, "This method should not be called");
}

void DepthWriteMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
	clear_command_.reset();
    context.materials[id()].clear();

    bool have_shadowcasters = false;
	for (size_t i = 0; i < render_context.lights_number; ++i)
	{
		if (!render_context.lights[i].light.desc().cast_shadows)
			continue;
		UniformBuffer& uniform = context.uniform_pool.get(uniform_);
		UniformData data;
		data.mvp = get_light_shadowmap_matrix(scene_context, render_context.lights[i].id);
		uniform.update(data);

		render_context.lights[i].light.set_shadowmap_texture(shadowmap_);

        have_shadowcasters = true;
	}

    if (!have_shadowcasters) return;

	for (size_t i = 0; i < render_context.nodes_number; ++i)
	{
        if (!render_context.nodes[i].cast_shadow && !render_context.nodes[i].receive_shadow)
			continue;

        for (size_t j = 0; j < render_context.nodes[i].mesh.instance_parts.size(); ++j)
        {
            const MeshPartInstance& part = render_context.nodes[i].mesh.instance_parts[j];
            DrawCall& draw_call = render_context.draw_calls.add();
            draw_call.render_data = render_context.nodes[i].mesh.mesh.parts[j].render_data;
            Material& material = create_and_get(context.materials[id()]);
            material.shader_program = default_program(context).id();
            material.uniforms[0] = uniform_;
            material.id = material.id;
            draw_call.material.material_system = id();
            draw_call.material.id = material.id;

            const MaterialInstance& original_material_instance = part.material;
            const Material& original_material = context.materials[original_material_instance.material_system].get(original_material_instance.id);
            material.uniforms[1] = original_material.uniforms[1];

            draw_call.draw_call_data = draw_call_data_;

            draw_call.command = &clear_command_;
        }
	}
}

}
