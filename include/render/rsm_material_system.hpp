// The implementation of Reflective Shadow Maps algorithm:
// http://www.vis.uni-stuttgart.de/~dachsbcn/download/rsm.pdf

#ifndef __RSM_MATERIAL_SYSTEM_HPP__
#define __RSM_MATERIAL_SYSTEM_HPP__

#include "gbuffer_material_system.hpp"
#include "rsm_common.hpp"

namespace mhe {

class RSMMaterialSystem : public MaterialSystem
{
    SETUP_MATERIAL("rsm");

    struct ShaderData
    {
        mat4x4 vp;
        vec4 settings;
        vec4 light_diffuse_color;
    };
public:
    enum
    {
        mode_scene,
        mode_volume
    };

    struct Settings
    {
        int mode;
        size_t size;
        float flux_intensity;
        AABBf volume_aabb;

        Settings() : mode(mode_scene), size(256), flux_intensity(1.0f) {}
    };
public:
    RSMMaterialSystem();

    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void destroy(Context& context) override;

    void setup(Context &context, SceneContext &scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;

    Settings& settings()
    {
        return settings_;
    }

    void start_frame(Context&, SceneContext&, RenderContext&) override;

    const GBuffer& gbuffer() const
    {
        return gbuffer_;
    }

    void rsm_data(RSMData& data) const
    {
        data.vp = light_vp_;
        data.light_direction = light_direction_;
        data.aabb = current_volume_;
    }

    void init_debug_views(Context& context) override;
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    Settings settings_;
    GBuffer gbuffer_;
    RenderTargetHandleType render_target_;
    UniformBufferHandleType transform_uniform_;
    RenderStateHandleType render_state_;
    mat4x4 light_view_;
    mat4x4 light_proj_;
    mat4x4 light_vp_;
    ClearCommand clear_command_;
    GPUProfileCommand profiler_command_;
    ListOfCommands list_of_commands_;
    ShaderData shader_data_;
    vec3 light_direction_;
    AABBf current_volume_;
};

}

#endif
