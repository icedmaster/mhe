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
        vec4 light_parameters;
    };
public:
    enum
    {
        mode_scene,
        mode_volume,
        mode_rsm
    };

    struct Settings
    {
        int mode;
        size_t size;
        size_t propagation_steps;
        float occlusion_coeff;
        float propagation_amp;
        float aabb_multiplier;
        AABBf volume;
        bool use_occlusion;

        Settings() :
            mode(mode_scene),
            size(32),
            propagation_steps(8),
            occlusion_coeff(1.0f),
            propagation_amp(4.0f),
            aabb_multiplier(1.0f),
            use_occlusion(true)
        {}
    };
public:
    LPVMaterialSystem();

    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void destroy(Context& context) override;

    void setup(Context &context, SceneContext &scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;

    void output(Context&, size_t unit, TextureInstance& texture) const override;

    void init_debug_views(Context& context) override;

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

    Settings& settings()
    {
        return settings_;
    }
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    void injection(DrawCall& draw_call, Context& context, RenderContext& render_context, size_t vpl_number);
    void geometry_injection(DrawCall& draw_call, Context& context, RenderContext& render_context, size_t vpl_number);
    void propagation(Context& context, RenderContext& render_context);

    Settings settings_;
    GBuffer gbuffer_;
    RenderTargetHandleType render_target_grid_;
    ShaderProgramHandleType injection_shader_;
    MaterialHandleType injection_material_;
    RenderStateHandleType injection_render_state_;
    UniformBufferHandleType injection_uniform_;
    ClearCommandSimple clear_command_;
    GPUProfileCommand injection_profile_command_;
    GPUProfileCommand geometry_injection_profile_command_;
    GPUProfileCommand propagation_profile_command_;
    ListOfCommands injection_list_of_commands_;
    ListOfCommands geometry_injection_list_of_commands_;
    ListOfCommands propagation_list_of_commands_;

    ShaderProgramHandleType propagation_shader_;
    ShaderProgramHandleType propagation_shader_with_occlusion_;
    RenderTargetHandleType propagation_render_target_;
    MaterialHandleType propagation_material_[2];

    RenderTargetHandleType occlusion_render_target_;
    MaterialHandleType geometry_injection_material_;
    ShaderProgramHandleType geometry_injection_shader_;

    RenderTargetHandleType output_render_target_;

    RSMData rsm_data_;
};

class LPVResolveMaterialSystem : public PosteffectMaterialSystemBase
{
    SETUP_POSTEFFECT_MATERIAL(LPVResolveMaterialSystem, "lpv_resolve");
public:
    struct Settings
    {
        bool damping;

        Settings() : damping(false) {}
    };

    bool init(Context& context, const MaterialSystemContext& material_system_context) override;

    Settings& settings()
    {
        return settings_;
    }
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    UberShader::Info damp_info_;
    Settings settings_;
};

}

#endif
