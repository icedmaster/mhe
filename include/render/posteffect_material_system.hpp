#ifndef __POSTEFFECT_MATERIAL_SYSTEM_HPP__
#define __POSTEFFECT_MATERIAL_SYSTEM_HPP__

#include "core/array.hpp"
#include "material_system.hpp"
#include "material.hpp"
#include "shader_program.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "commands.hpp"
#include "render_common.hpp"
#include "draw_call.hpp"

namespace mhe {

class PosteffectDebugMaterialSystem : public MaterialSystem
{
    SETUP_MATERIAL("posteffect_debug");
private:
    static const size_t max_textures_number = 4;
public:
    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void close() override;

    void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;

    void set_texture(size_t unit, const TextureInstance& texture)
    {
        ASSERT(unit < max_textures_number, "Invalid unit for texture");
        textures_[unit] = texture;
    }

    void set_render_target(const RenderTarget& render_target);

    void set_viewports_number(size_t number)
    {
        textures_number_ = number;
    }
private:
    bool init_mesh(Context& context, const MaterialSystemContext& material_system_context);
    bool init_mesh_instance(Context& context, MeshInstance& mesh_instance, const rect<int>& viewport, bool overdraw);

    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    ClearCommand clear_command_;
    MeshInstance mesh_[max_textures_number];
    TextureInstance textures_[max_textures_number];
    MeshInstance fullscreen_mesh_;
    TextureInstance fullscreen_texture_;
    uint8_t texture_type_mask_;
    uint8_t textures_number_;
};

class PosteffectMaterialSystemBase : public MaterialSystem
{
    static const size_t max_textures_number = 8;
    static const size_t max_buffers = 4;
    static const size_t invalid_index = static_cast<size_t>(-1);
public:
    PosteffectMaterialSystemBase(const char* name);

    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void close() override;

    void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;

    size_t default_instances_number() const override
    {
        return 1;
    }

    size_t inputs_number() const
    {
        return inputs_number_;
    }

    size_t outputs_number() const
    {
        return outputs_number_;
    }

    TextureInstance& input(size_t index)
    {
        return inputs_[index];
    }

    TextureInstance& output(size_t index)
    {
        return outputs_[index];
    }

    void output(Context&, size_t unit, TextureInstance& texture) const override
    {
        texture = outputs_[unit];
    }

    void set_input(size_t index, const TextureInstance& texture)
    {
        inputs_[index] = texture;
        inputs_number_ = max(inputs_number_, index) + 1;
    }

    void set_output(size_t index, const TextureInstance& texture)
    {
        outputs_[index] = texture;
        outputs_number_ = max(outputs_number_, index) + 1;
    }

    void set_inputs_number(size_t inputs)
    {
        inputs_number_ = inputs;
    }

    void set_outputs_number(size_t outputs)
    {
        outputs_number_ = outputs;
    }

    void set_inputs(size_t number, const TextureInstance* textures)
    {
        for (size_t i = 0; i < number; ++i)
            inputs_[i] = textures[i];
        inputs_number_ = number;
    }

    void set_outputs(size_t number, const TextureInstance* textures)
    {
        for (size_t i = 0; i < number; ++i)
            outputs_[i] = textures[i];
        outputs_number_ = number;
    }

    bool init_screen_input(Context& context, size_t index, uint8_t render_stage = render_stage_before_render_target_setup);
    bool create_output(Context& context, size_t index, float scale, int format);

    // Currently, SSBOs and UBOs are stored separately but applied together (through Material.uniforms)
    void set_buffer(size_t index, ShaderStorageBufferHandleType id)
    {
        buffers_[index] = id;
    }

    ShaderProgramHandleType buffer(size_t index) const
    {
        return buffers_[index];
    }

    void set_uniform(size_t index, UniformBufferHandleType id)
    {
        uniforms_[index] = id;
    }

    UniformBufferHandleType uniform(size_t index) const
    {
        return uniforms_[index];
    }

    // This method will be called when PosteffectChain has initialized this material system
    virtual void postinit(Context& /*context*/) {}
protected:
    MeshInstance& mesh_instance()
    {
        return mesh_;
    }

    Material& default_material(Context& context);

    void fill_render_target_desc(RenderTargetDesc& desc, int format = format_rgba) const;

    virtual bool create_output(DrawCallData& draw_call_data, Context& context, size_t index, float scale, int format);
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;
    void prepare_draw_call(DrawCall& draw_call, Context& context, SceneContext& scene_context, const RenderContext& render_context);
    RenderTargetHandleType default_render_target(Context& context) const;
private:
    bool init_mesh(Context& context, const MaterialSystemContext& material_system_context);

    array<TextureInstance, max_textures_number> inputs_;
    array<TextureInstance, max_textures_number> outputs_;
    array<ShaderProgramHandleType, max_buffers> buffers_;
    array<UniformBufferHandleType, max_buffers> uniforms_;
    size_t inputs_number_;
    size_t outputs_number_;
    MeshInstance mesh_;
    ClearCommand clear_command_;
    CopyFramebufferCommand copy_framebuffer_command_;
    ListOfCommands list_of_commands_;
    GPUProfileCommand profile_command_;
    size_t framebuffer_input_;
    UberShader::Info inputs_number_shader_info_;
};

#define SETUP_POSTEFFECT_MATERIAL(class_name, name)                 \
    public: class_name() : PosteffectMaterialSystemBase(name) {} \
    private: SETUP_MATERIAL(name)

class PosteffectSimpleMaterialSystem : public PosteffectMaterialSystemBase
{
    SETUP_POSTEFFECT_MATERIAL(PosteffectSimpleMaterialSystem, "posteffect_simple");
};

class TonemapMaterialSystem : public PosteffectMaterialSystemBase
{
    SETUP_POSTEFFECT_MATERIAL(TonemapMaterialSystem, "tonemap");
};

class FXAAMaterialSystem : public PosteffectMaterialSystemBase
{
    SETUP_POSTEFFECT_MATERIAL(FXAAMaterialSystem, "fxaa");
};

class SSRMaterialSystem : public PosteffectMaterialSystemBase
{
    SETUP_POSTEFFECT_MATERIAL(SSRMaterialSystem, "ssr");

    static const size_t probes_texture_unit = 5;
public:
    struct Settings
    {
        bool use_probes;

        Settings() : use_probes(false) {}
    };

    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void init_debug_views(Context& context) override;

    Settings& settings()
    {
        return settings_;
    }
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    struct SSRShaderData
    {
        vec4 ssrdata[3];
    };

    Settings settings_;
    SSRShaderData ssr_shader_data_;
    UniformBuffer::IdType ssr_uniform_;
    UberShader::Info probes_info_;
};

class BlurMaterialSystem : public PosteffectMaterialSystemBase
{
    SETUP_POSTEFFECT_MATERIAL(BlurMaterialSystem, "blur");

    struct ShaderData
    {
        vec4 params;
    };
public:
    enum
    {
        quality_normal,
        quality_high
    };

    struct Settings
    {
        float size;
        int quality;

        Settings() : size(1.0f), quality(quality_high) {}
    };

    bool init(Context& context, const MaterialSystemContext& material_system_context) override;

    size_t default_instances_number() const override
    {
        return 2;
    }

    Settings& settings()
    {
        return settings_;
    }

    void set_quality(int quality)
    {
        settings_.quality = quality;
    }

    void setup_draw_calls(Context& context, SceneContext& scene_context, DrawCall* draw_calls, size_t draw_calls_number,
        const RenderContext& render_context);

    RenderTarget::IdType render_target_id() const override
    {
        return output_rt_;
    }
private:
    bool create_output(DrawCallData& draw_call_data, Context& context, size_t index, float scale, int format) override;
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    Settings settings_;
    UniformBuffer::IdType uniform_;
    UberShader::Info pass_info_;
    UberShader::Info quality_info_;
    MeshInstance second_pass_mesh_;
    ClearCommand clear_command_;
    RenderTargetHandleType output_rt_;
};

class DOFMaterialSystem : public PosteffectMaterialSystemBase
{
    SETUP_POSTEFFECT_MATERIAL(DOFMaterialSystem, "dof");

    static const size_t input_texture_unit = 4;
    static const size_t blur_texture_unit = 5;
    static const size_t dof_texture_unit = 6;

    struct DOFShaderData
    {
        vec4 planes;
        vec4 coc;
    };
public:
    bool init(Context& context, const MaterialSystemContext& material_system_context) override;

    size_t default_instances_number() const override
    {
        return 3; // blur resolve pass + DOF pass + composite pass
    }

    void postinit(Context& context) override;
    void init_debug_views(Context& context) override;
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;
    void update_uniforms(Context& context);

    ClearCommandSimple clear_command_simple_;
    MeshInstance dof_pass_mesh_instance_;
    MeshInstance composite_pass_mesh_instance_;
    UberShader::Info pass_info_;
    float blur_resolve_pass_scale_;

    DOFShaderData dof_shader_data_;
    UniformBuffer::IdType dof_uniform_;
};

class SSAOMaterialSystem : public PosteffectMaterialSystemBase
{
    SETUP_POSTEFFECT_MATERIAL(SSAOMaterialSystem, "ssao");

    static const size_t noise_texture_unit = 4;
public:
    struct Settings
    {
        float rotation_speed;

        Settings() :
            rotation_speed(1.0f)
        {}
    };

    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void init_debug_views(Context& context) override;
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;
    void create_noise_texture(TextureInstance& texture, Context& context);

    struct SSAOShaderData
    {
        vec4 ssaodata[3];
    };

    Settings settings_;
    SSAOShaderData ssao_shader_data_;
    UniformBuffer::IdType ssao_uniform_;
    TextureInstance noise_texture_;
};

class CompositeMulMaterialSystem : public PosteffectMaterialSystemBase
{
    SETUP_POSTEFFECT_MATERIAL(CompositeMulMaterialSystem, "comp_mul");
};

class CompositeAddMaterialSystem : public PosteffectMaterialSystemBase
{
    SETUP_POSTEFFECT_MATERIAL(CompositeAddMaterialSystem, "comp_add");
};

class AverageLuminanceMaterialSystem : public PosteffectMaterialSystemBase
{
    SETUP_POSTEFFECT_MATERIAL(AverageLuminanceMaterialSystem, "average_luminance");

    struct AdaptationShaderData
    {
        vec4 data;
    };

    static const size_t reduction_threads_number = 32;

    struct ReductionContext
    {
        size_t input_size;
        TextureInstance input;
        ShaderProgramHandleType reduction_shader;
        ShaderProgramHandleType texture_to_buffer_shader;
        ShaderProgramHandleType buffer_to_texture_shader;
        TextureInstance output;
    };

    class ReductionCommand : public RenderCommand
    {
    public:
        ReductionCommand();

        void set_parameters(ShaderStorageBufferHandleType* buffers, const ReductionContext& reduction_context, size_t threads_number);
        void set_input(const TextureInstance& texture)
        {
            reduction_context_.input = texture;
        }
    private:
        bool execute_impl(Context& context, RenderStage current_stage) override;

        ReductionContext reduction_context_;
        ComputeCallExplicit compute_call_;
        ShaderStorageBufferHandleType buffers_[2];
        size_t threads_number_;
    };
public:
    struct Settings
    {
        float adaptation_rate;

        Settings() : adaptation_rate(1.0f) {}
    };

    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void init_debug_views(Context& context) override;
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    size_t default_instances_number() const override
    {
        return 2;
    }

    Settings& settings()
    {
        return settings_;
    }
private:
    Settings settings_;
    ShaderStorageBufferHandleType shader_storage_[2];
    ShaderProgramHandleType reduction_shader_;
    ShaderProgramHandleType texture_to_buffer_shader_;
    ShaderProgramHandleType adaptation_shader_;
    UniformBufferHandleType adaptation_uniform_;
    ReductionCommand reduction_command_;
    RenderTargetHandleType adaptation_render_target_[2];
    MaterialHandleType adaptation_material_;
    DrawCallData adaptation_draw_call_data_;
    uint8_t adaptation_rt_index_;
};

class BloomMaterialSystem : public PosteffectMaterialSystemBase
{
    SETUP_POSTEFFECT_MATERIAL(BloomMaterialSystem, "bloom");

    static const size_t steps_number = 6;

    struct ShaderData
    {
        vec4 settings;
    };
public:
    struct Settings
    {
        float exposure_key;
        float threshold;
        float intensity;
    };

    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void init_debug_views(Context& context) override;

    size_t default_instances_number() const override
    {
        return steps_number + 1;
    }
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;
    void copy(Context& context, DrawCall& draw_call, RenderTarget& dst, const RenderTarget& from, size_t pass);

    Settings settings_;
    UniformBufferHandleType uniform_;
    DrawCallData draw_calls_data_[steps_number];
    MaterialHandleType materials_[steps_number];
    BlurMaterialSystem* blur_material_system_;
    ClearCommandSimple clear_command_simple_;
};

}

#endif
