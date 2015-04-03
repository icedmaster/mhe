#include "render/deferred_renderer.hpp"

#include "core/assert.hpp"
#include "render/context.hpp"

namespace mhe {

void DeferredRenderer::init(AbstractGBufferFillMaterialSystem* fill, AbstractGBufferUseMaterialSystem* light, MaterialSystem* draw)
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
		draw->set_texture(textures[0]);
		if (light)
			draw->set_texture(1, light->lighting_texture());
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

}
