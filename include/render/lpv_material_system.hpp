#ifndef __LPV_MATERIAL_SYSTEM_HPP__
#define __LPV_MATERIAL_SYSTEM_HPP__

#include "gbuffer_material_system.hpp"
#include "posteffect_material_system.hpp"
#include "rsm_common.hpp"

namespace mhe {

class LPVMaterialSystem : public MaterialSystem
{
    SETUP_MATERIAL("lpv");

    struct InjectionShaderData
    {
        vec4 settings;
        mat4x4 rsm_to_ws;
        mat4x4 ws_to_lpv;
    };
public:
    struct Settings
    {
        size_t size;
        size_t propagation_steps;

        Settings() :
            size(32),
            propagation_steps(1)
        {}
    };
public:
    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void destroy(Context& context) override;

    void setup(Context &context, SceneContext &scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;

    void output(Context&, size_t unit, TextureInstance& texture) const override;

    void set_gbuffer(const GBuffer& gbuffer)
    {
        gbuffer_ = gbuffer;
    }

    void set_rsm_data(const RSMData& data)
    {
        rsm_data_ = data;
    }

    UniformBufferHandleType injection_settings_uniform() const
    {
        return injection_uniform_;
    }
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    void injection(DrawCall& draw_call, Context& context, RenderContext& render_context, size_t vpl_number);
    void propagation(Context& context, RenderContext& render_context);
    mat4x4 calculate_lpv_transform(const RenderContext& render_context);

    Settings settings_;
    GBuffer gbuffer_;
    RenderTargetHandleType render_target_grid_;
    ShaderProgramHandleType injection_shader_;
    MaterialHandleType injection_material_;
    RenderStateHandleType injection_render_state_;
    UniformBufferHandleType injection_uniform_;
    ClearCommandSimple clear_command_;

    ShaderProgramHandleType propagation_shader_;
    RenderTargetHandleType propagation_render_target_;
    MaterialHandleType propagation_material_[2];

    RenderTargetHandleType output_render_target_;

    RSMData rsm_data_;
};

class LPVResolveMaterialSystem : public PosteffectMaterialSystemBase
{
    SETUP_POSTEFFECT_MATERIAL(LPVResolveMaterialSystem, "lpv_resolve");
};

}

#endif
