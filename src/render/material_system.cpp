#include "render/material_system.hpp"

#include "render/context.hpp"
#include "render/instances.hpp"
#include "render/render_context.hpp"
#include "render/scene_context.hpp"
#include "render/uniforms.hpp"

namespace mhe {

void MaterialSystem::setup_draw_call(DrawCall& draw_call, const MeshPartInstance& instance_part, const MeshPart& part, RenderCommand* command) const
{
    draw_call.material = instance_part.material;
    draw_call.draw_call_data = instance_part.draw_call_data;
    draw_call.render_data = part.render_data;
    draw_call.command = command;

    ASSERT(draw_call.material.material_system != invalid_material_system_id, "Invalid draw call");
}

void MaterialSystem::setup_draw_calls(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    if (!enabled_) return;
    update(context, scene_context, render_context);
}

bool MaterialSystem::init_default(Context& context, const MaterialSystemContext& material_system_context)
{
	instance_name_ = material_system_context.instance_name;
	return context.shader_manager.get(shader_, material_system_context.shader_name);
}

void MaterialSystem::standart_material_setup(Context& context, SceneContext& scene_context,
                                             MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
	if (!count) return;
    ASSERT(parts, "Invalid materials parameter");
	ASSERT(model_contexts, "Invalid model contexts");

	size_t layout_id = layout();
	size_t material_system_id = id();
	size_t shader_id = ubershader(context).get_default();

	for (size_t i = 0; i < count; ++i)
	{
		Material::IdType material_id = context.materials[id()].create();
		Material& material = context.materials[id()].get(material_id);
		material.shader_program = shader_id;
		setup_uniforms(material, context, scene_context, instance_parts[i], model_contexts[i]);

		instance_parts[i].material.material_system = material_system_id;
		instance_parts[i].material.id = material.id;

		MaterialData material_data;
		if (!context.material_manager.get(material_data, parts[i].material_id))
		{
			ERROR_LOG("Can't find material with id:" << parts[i].material_id);
			continue;
		}

		::memcpy(material.textures, material_data.textures, sizeof(TextureInstance) * material_textures_number);

		if (parts[i].render_data.layout == Layout::invalid_id)
			parts[i].render_data.layout = layout_id;

		DrawCallData& draw_call_data = create_and_get(context.draw_call_data_pool);
		RenderState& render_state = create_and_get(context.render_state_pool);
		RenderStateDesc desc;
		render_state.init(desc);
		draw_call_data.state = render_state.id();
		instance_parts[i].draw_call_data = draw_call_data.id;
	}
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

void MaterialSystem::empty_setup(Context& /*context*/, SceneContext& /*scene_context*/, MeshPartInstance* /*parts*/, MeshPart* /*parts*/, ModelContext* /*model_contexts*/, size_t /*count*/)
{
    ASSERT(0, "This method should not be called");
}

void MaterialSystem::empty_update(Context& /*context*/, SceneContext& /*scene_context*/, RenderContext& /*render_context*/, MeshPartInstance* /*parts*/, size_t /*count*/)
{
    ASSERT(0, "This method should not be called");
}

}
