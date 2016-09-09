#include "render/deferred_renderer.hpp"

#include "core/assert.hpp"
#include "render/context.hpp"
#include "render/posteffect_material_system.hpp"
#include "render/uniforms.hpp"

namespace mhe {

void DeferredRenderer::init(AbstractGBufferFillMaterialSystem* fill, AbstractGBufferUseMaterialSystem* light,
    PosteffectMaterialSystemBase* draw, const Renderer::Settings& settings)
{
    Renderer::init(settings);
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

void DeferredRenderer::set_gi_modifier_material_system(MaterialSystem* material_system, size_t priority)
{
    gi_modifier_material_system_ = material_system;
    if (material_system != nullptr)
    {
        TextureInstance accumulator_texture;
        material_system->output(context(), 0, accumulator_texture);
        draw_material_system_->set_input(2, accumulator_texture);
        material_system->set_priority(deferred_renderer_gbuffer_modifier_priority + priority);
    }
}

void DeferredRenderer::setup_common_pass(Material& material) const
{
    material.textures[0] = gbuffer_.albedo;
    material.textures[1] = gbuffer_.normal;
    material.textures[2] = gbuffer_.accumulator;
    material.textures[3] = gbuffer_.depth;
    material.uniforms[0] = main_camera_uniform();
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
        gbuffer_light_material_system_->set_priority(deferred_renderer_draw_priority);
    if (draw_material_system_ != nullptr)
        draw_material_system_->set_priority(deferred_renderer_draw_priority + 1); // reserve priorities for GBuffer modificators
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

}
