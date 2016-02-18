#include "render/deferred_renderer.hpp"

#include "core/assert.hpp"
#include "render/context.hpp"
#include "render/posteffect_material_system.hpp"
#include "render/uniforms.hpp"

namespace mhe {

void DeferredRenderer::init(AbstractGBufferFillMaterialSystem* fill, AbstractGBufferUseMaterialSystem* light, PosteffectMaterialSystemBase* draw)
{
    Renderer::init();
    ASSERT(fill != nullptr, "DeferredRenderer requires fill material system");
    gbuffer_fill_material_system_ = fill;
    gbuffer_light_material_system_ = light;
    draw_material_system_ = draw;

    init_priorities();

    RenderTarget& render_target = context().render_target_pool.get(fill->render_target());
    if (light != nullptr)
        light->set_render_target(context(), fill->render_target());
    const TextureInstance* textures = nullptr;
    if (draw != nullptr)
    {
        size_t outputs_number = render_target.color_textures(&textures);
        ASSERT(outputs_number == 3, "GBuffer has to have some color textures");
        draw->set_inputs(outputs_number, textures);
        TextureInstance depth_texture;
        size_t depth_outputs = render_target.depth_texture(depth_texture);
        ASSERT(depth_outputs != 0, "GBuffer has to have a depth texture");
        draw->set_input(gbuffer_depth_render_target_index, depth_texture);
        if (light)
            draw->set_input(gbuffer_depth_render_target_index + 1, light->lighting_texture());
        // gbuffer draw system writes final image to the HDR buffer which must be postprocessed later
        // TODO: Do we need some setting in the config file in order to understand whether to write result
        // to the screen of to texture?
        draw->create_output(context(), 0, 1.0f, format_rgba16f);

        gbuffer_.albedo = textures[0];
        gbuffer_.normal = textures[1];
        gbuffer_.accumulator = textures[2];
        gbuffer_.depth = depth_texture;
        gbuffer_.lighting = light->lighting_texture();
    }
}

void DeferredRenderer::set_probes_accumulator_material_system(ProbesAccumulatorMaterialSystem* material_system)
{
    probes_accumulator_material_system_ = material_system;
    if (probes_accumulator_material_system_ != nullptr)
    {
        probes_accumulator_material_system_->set_priority(deferred_renderer_draw_priority + 1);
        probes_accumulator_material_system_->set_gbuffer(gbuffer_);
    }
}

void DeferredRenderer::set_gi_modifier_material_system(Context& context, MaterialSystem* material_system, size_t priority)
{
    gi_modifier_material_system_ = material_system;
    if (material_system != nullptr)
    {
        TextureInstance accumulator_texture;
        material_system->output(context, 0, accumulator_texture);
        draw_material_system_->set_input(2, accumulator_texture);
        material_system->set_priority(deferred_renderer_gbuffer_modifier_priority + priority);
    }
}

void DeferredRenderer::enable()
{
    gbuffer_fill_material_system_->enable();
    if (gbuffer_light_material_system_ != nullptr)
        gbuffer_light_material_system_->enable();
    if (draw_material_system_ != nullptr)
        draw_material_system_->enable();
    if (probes_accumulator_material_system_ != nullptr)
        probes_accumulator_material_system_->enable();
    if (gi_modifier_material_system_ != nullptr)
        gi_modifier_material_system_->enable();
}

void DeferredRenderer::disable()
{
    gbuffer_fill_material_system_->disable();
    if (gbuffer_light_material_system_ != nullptr)
        gbuffer_light_material_system_->disable();
    if (draw_material_system_ != nullptr)
        draw_material_system_->disable();
    if (probes_accumulator_material_system_ != nullptr)
        probes_accumulator_material_system_->disable();
    if (gi_modifier_material_system_ != nullptr)
        gi_modifier_material_system_->disable();
}

void DeferredRenderer::init_priorities()
{
    gbuffer_fill_material_system_->set_priority(deferred_renderer_base_priority);
    if (gbuffer_light_material_system_ != nullptr)
        gbuffer_light_material_system_->set_priority(deferred_renderer_base_priority + 1);
    if (draw_material_system_ != nullptr)
        draw_material_system_->set_priority(deferred_renderer_draw_priority); // reserve priorities for GBuffer modificators
}

void DeferredRenderer::render_impl(Context& context, RenderContext& render_context, SceneContext& scene_context)
{
    gbuffer_fill_material_system_->setup_draw_calls(context, scene_context, render_context);
    if (gbuffer_light_material_system_ != nullptr)
        gbuffer_light_material_system_->setup_draw_calls(context, scene_context, render_context);
    if (draw_material_system_ != nullptr)
        draw_material_system_->setup_draw_calls(context, scene_context, render_context);
    if (probes_accumulator_material_system_ != nullptr)
        probes_accumulator_material_system_->setup_draw_calls(context, scene_context, render_context);
    if (gi_modifier_material_system_ != nullptr)
        gi_modifier_material_system_->setup_draw_calls(context, scene_context, render_context);
}

void DeferredRenderer::debug_mode_changed(DebugMode mode, MaterialSystemId material_system_id)
{
    Renderer::debug_mode_changed(mode, material_system_id);
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
    else if (mode == Renderer::renderer_debug_mode_baked_irradiance)
    {
        RenderTarget& render_target = context().render_target_pool.get(gbuffer_fill_material_system_->render_target());
        TextureInstance baked_irradiance_texture;
        render_target.color_texture(baked_irradiance_texture, baked_light_texture_unit);
        debug_material_system->set_texture(0, baked_irradiance_texture);
        debug_material_system->enable();
    }
    else if (mode == Renderer::renderer_debug_mode_probes && probes_accumulator_material_system_ != nullptr)
    {
        RenderTarget& render_target = context().render_target_pool.get(probes_accumulator_material_system_->render_target());
        TextureInstance texture;
        render_target.color_texture(texture, 0);
        debug_material_system->set_texture(0, texture);
        debug_material_system->enable();
    }
}

}
