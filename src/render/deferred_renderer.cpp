#include "render/deferred_renderer.hpp"

#include "core/assert.hpp"
#include "render/context.hpp"
#include "render/posteffect_material_system.hpp"

namespace mhe {

void DeferredRenderer::init(AbstractGBufferFillMaterialSystem* fill, AbstractGBufferUseMaterialSystem* light, PosteffectMaterialSystemBase* draw)
{
	ASSERT(fill != nullptr, "DeferredRenderer requires fill material system");
	gbuffer_fill_material_system_ = fill;
	gbuffer_light_material_system_ = light;
	draw_material_system_ = draw;

	init_priorities();

	RenderTarget& render_target = context().render_target_pool.get(fill->render_target());
	if (light != nullptr)
		light->set_render_target(context(), fill->render_target());
	const TextureInstance* textures = nullptr;
	if (draw != nullptr && render_target.color_textures(&textures))
	{
		draw->set_input(0, textures[0]);
		if (light)
			draw->set_input(1, light->lighting_texture());
	}
}

void DeferredRenderer::init_priorities()
{
	gbuffer_fill_material_system_->set_priority(4);
	if (gbuffer_light_material_system_ != nullptr)
		gbuffer_light_material_system_->set_priority(5);
	if (draw_material_system_ != nullptr)
		draw_material_system_->set_priority(6);
}

void DeferredRenderer::render_impl(Context& context, RenderContext& render_context, SceneContext& scene_context)
{
	gbuffer_fill_material_system_->setup_draw_calls(context, scene_context, render_context);
	if (gbuffer_light_material_system_ != nullptr)
		gbuffer_light_material_system_->setup_draw_calls(context, scene_context, render_context);
	if (draw_material_system_ != nullptr)
		draw_material_system_->setup_draw_calls(context, scene_context, render_context);
}

void DeferredRenderer::debug_mode_changed(DebugMode mode)
{
	Renderer::debug_mode_changed(mode);
	PosteffectDebugMaterialSystem* debug_material_system = context().material_systems.get<PosteffectDebugMaterialSystem>();
	ASSERT(debug_material_system != nullptr, "Invalid DebugMaterialSystem");
	if (mode == Renderer::renderer_debug_mode_none)
		debug_material_system->disable();
	else if (mode == Renderer::renderer_debug_mode_main)
	{
		debug_material_system->set_render_target(context().render_target_pool.get(gbuffer_fill_material_system_->render_target()));
		debug_material_system->set_texture(2, gbuffer_light_material_system_->lighting_texture());
		debug_material_system->enable();
	}
}

}
