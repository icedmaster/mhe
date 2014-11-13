#include "render/material_system.hpp"

#include "render/context.hpp"
#include "render/instances.hpp"
#include "render/render_context.hpp"
#include "render/scene_context.hpp"

namespace mhe {

void MaterialSystem::standart_material_setup(Context& context, SceneContext& scene_context, NodeInstance* nodes, ModelContext* model_contexts, size_t count,
											size_t textures_number)
{
	if (!count) return;
	ASSERT(nodes, "Invalid materials parameter");
	ASSERT(model_contexts, "Invalid model contexts");

	size_t layout_id = layout();
	size_t material_system_id = id();
	size_t shader_id = ubershader(context).get_default();

	for (size_t i = 0; i < count; ++i)
	{
		Material::IdType material_id = context.materials[id()].create();
		Material& material = context.materials[id()].get(material_id);
		material.shader_program = shader_id;
		setup_uniforms(material, context, scene_context, nodes[i], model_contexts[i]);

		nodes[i].node.main_pass.material.material_system = material_system_id;
		nodes[i].node.main_pass.material.id = material.id;

		nodes[i].node.mesh.layout = layout_id;

		for (size_t j = 0; j < textures_number; ++j)
		{
			if (model_contexts[i].textures[j].empty()) continue;
			context.texture_manager.get(material.textures[j], model_contexts[i].textures[j]);
		}

		DrawCallData& draw_call_data = create_and_get(context.draw_call_data_pool);
		RenderState& render_state = create_and_get(context.render_state_pool);
		RenderStateDesc desc;
		render_state.init(desc);
		draw_call_data.state = render_state.id();
		nodes[i].node.main_pass.draw_call_data = draw_call_data.id;

		context.additional_passes_pool.make_invalid(nodes[i].node.additional_passes);
	}
}

void MaterialSystem::additional_passes_setup(Context& /*context*/, NodeInstance* /*nodes*/, size_t /*count*/)
{
}

Transform& MaterialSystem::transform(const NodeInstance& node, const SceneContext& scene_context) const
{
	return scene_context.transform_pool.get(node.transform_id).transform;
}

UberShader& MaterialSystem::ubershader(const Context& context) const
{
	return context.ubershader_pool.get(shader_.shader_program_handle);
}

ShaderProgram& MaterialSystem::default_program(const Context& context) const
{
	return context.shader_pool.get(ubershader(context).get_default());
}

}
