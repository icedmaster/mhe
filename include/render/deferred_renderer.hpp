#ifndef __DEFERRED_RENDERER_HPP__
#define __DEFERRED_RENDERER_HPP__

#include "renderer.hpp"
#include "gbuffer_material_system.hpp"

namespace mhe {

class MHE_EXPORT DeferredRenderer : public Renderer
{
public:
    static const uint8_t deferred_renderer_base_priority = 16;
    static const uint8_t deferred_renderer_gbuffer_modifier_priority = 20;
    static const uint8_t deferred_renderer_draw_priority = 32;
public:
    DeferredRenderer(Context& context) :
        Renderer(context),
        gi_modifier_material_system_(nullptr)
    {}

    void init(AbstractGBufferFillMaterialSystem* fill, AbstractGBufferUseMaterialSystem* light, PosteffectMaterialSystemBase* draw,
        const Renderer::Settings& settings);
    void enable();
    void disable();

    void set_probes_accumulator_material_system(ProbesAccumulatorMaterialSystem* material_system);
    void set_gi_modifier_material_system(MaterialSystem* material_system, size_t priority) override;

    TextureInstance scene_normals_buffer() const override
    {
        return gbuffer_.normal;
    }

    TextureInstance scene_depth_buffer() const override
    {
        return gbuffer_.depth;
    }

    void setup_common_pass(Material& material) const override;
private:
    void init_priorities();

    void render_impl(Context& context, RenderContext& render_context, SceneContext& scene_context) override;
    void debug_mode_changed(DebugMode mode, MaterialSystemId material_system_id) override;

    GBuffer gbuffer_;
    MaterialSystem* shadowmap_depth_write_material_system_;
    AbstractGBufferFillMaterialSystem* gbuffer_fill_material_system_;
    AbstractGBufferUseMaterialSystem* gbuffer_light_material_system_;
    PosteffectMaterialSystemBase* draw_material_system_;
    ProbesAccumulatorMaterialSystem* probes_accumulator_material_system_;
    MaterialSystem* gi_modifier_material_system_;
};

}

#endif
