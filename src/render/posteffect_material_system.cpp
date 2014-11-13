#include "render/posteffect_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/instances.hpp"

namespace mhe {

bool PosteffectSimpleMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	FullscreenLayout::init(context);
	set_layout(FullscreenLayout::handle);

	return context.shader_manager.get(shader(), material_system_context.shader_name);
}

void PosteffectSimpleMaterialSystem::close()
{
}

void PosteffectSimpleMaterialSystem::setup(Context& context, SceneContext& scene_context, NodeInstance* nodes, ModelContext* model_contexts, size_t count)
{
	standart_material_setup(context, scene_context, nodes, model_contexts, count, 0);
	for (size_t i = 0; i < count; ++i)
	{
		Material& material = context.materials[id()].get(nodes[i].node.main_pass.material.id);

        for (size_t j = 0; j < material_textures_number; ++j)
		{
            if (textures_[j].id == Texture::invalid_id)
				continue;
            material.textures[j] = textures_[j];
		}

		DrawCallData& draw_call_data = context.draw_call_data_pool.get(nodes[i].node.main_pass.draw_call_data);
		RenderStateDesc desc;
		desc.blend.enabled = true;
		desc.blend.srcmode = blend_src_alpha;
		desc.blend.dstmode = blend_src_inv_alpha;
		desc.depth.enabled = false;
		RenderState& state = context.render_state_pool.get(draw_call_data.state);
		state.update(desc);
	}
}

void PosteffectSimpleMaterialSystem::destroy(Context& /*context*/, SceneContext& /*scene_context*/, NodeInstance* /*nodes*/, size_t /*count*/)
{
}

void PosteffectSimpleMaterialSystem::update(Context& /*context*/, SceneContext& /*scene_context*/, RenderContext& /*render_context*/, NodeInstance* /*nodes*/, size_t /*count*/)
{
}

}
