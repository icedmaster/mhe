#ifndef __FOG_HPP__
#define __FOG_HPP__

#include "posteffect_material_system.hpp"
#include "render_common.hpp"
#include "gbuffer_material_system.hpp"
#include "commands.hpp"

namespace mhe {

struct ShadowInfo;
class PosteffectSimpleMaterialSystem;

class HeightFogMaterialSystem : public PosteffectMaterialSystemBase
{
    SETUP_POSTEFFECT_MATERIAL(HeightFogMaterialSystem, "height_fog");

    struct ShaderData
    {
        vec4 param1;
        vec4 fog_color;
    };
public:
    struct Settings
    {
        float start;
        float density;
        float falloff;
        colorf color;
    };
public:
    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void destroy(Context& context) override;

    Settings& settings()
    {
        return settings_;
    }
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    Settings settings_;
    UniformBufferHandleType uniform_buffer_id_;
    RenderStateHandleType render_state_id_;
};

class ExponentialShadowMap : public MaterialSystem
{
    SETUP_MATERIAL("esm")

    struct ShaderData
    {
        vec4 settings;
    };

    class DownsampleCommand : public RenderCommand
    {
    public:
        struct Settings
        {
            ShaderProgramHandleType shader;
            TextureInstance source_texture;
            TextureInstance dest_texture;
            UniformBufferHandleType uniform;
            uivec3 threads_number;
        };

        bool init(const Settings& settings);
    private:
        bool execute_impl(Context& context, RenderStage current_stage) override;

        Settings settings_;
    };
public:
    struct Settings
    {
        float exponent;

        Settings() : exponent(10.0f) {}
    };
public:
    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void destroy(Context& context) override;

    void setup(Context &context, SceneContext &scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;

    Settings& settings()
    {
        return settings_;
    }

    MHE_EXPORT TextureInstance shadow_texture(Context& context) const;
    MHE_EXPORT LightInstanceIdType light_instance_id() const
    {
        return light_instance_id_;
    }
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    void init_downsampling(Context& context, const ShadowInfo* shadow_info);

    Settings settings_;
    DownsampleCommand downsample_command_;
    MeshInstance mesh_instance_;
    size_t esm_size_;
    size_t cs_threads_number_;
    ShaderProgramHandleType downsample_shader_program_id_;
    TextureInstance downsampled_texture_;
    RenderTargetHandleType resolved_shadow_rt_;
    UniformBufferHandleType uniform_id_;
    PosteffectSimpleMaterialSystem* depth_copy_material_system_;
    RenderTargetHandleType shadowmap_;
    LightInstanceIdType light_instance_id_;
};

class VolumetricFogMaterialSystem : public MaterialSystem
{
    SETUP_MATERIAL("volumetric_fog")
private:
    struct ShaderData
    {
        vec4 volume_size;
        vec4 fog_color;
    };
public:
    struct Settings
    {
        colorf color;
        float range;

        Settings() :
            color(0.5f, 0.6f, 0.7f, 0.5f),
            range(150.0f)
        {}
    };
public:
    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void destroy(Context& context) override;

    void setup(Context &context, SceneContext &scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;

    void set_light_instance(LightInstanceIdType instance_id, const TextureInstance& shadow_texture)
    {
        light_instance_id_ = instance_id;
        shadow_texture_ = shadow_texture;
    }

    Settings& settings()
    {
        return settings_;
    }
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    Settings settings_;
    uivec3 volume_size_;
    LightInstanceIdType light_instance_id_;
    TextureInstance volume_textures_[2];
    TextureInstance shadow_texture_;
    UniformBufferHandleType uniform_id_;
    ComputeCallCommand compute_call_command_;
    ComputeCallCommand propagation_command_;
    ListOfCommands list_of_commands_;
    ShaderProgramHandleType propagation_shader_;
};

}

#endif
