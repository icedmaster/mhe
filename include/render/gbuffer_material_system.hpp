#ifndef __GBUFFER_MATERIAL_SYSTEM_HPP__
#define __GBUFFER_MATERIAL_SYSTEM_HPP__

#include "material_system.hpp"
#include "shader_program.hpp"
#include "render_buffer.hpp"
#include "render_target.hpp"
#include "node.hpp"
#include "commands.hpp"
#include "math/matrix.hpp"

#include "debug/rdbg.hpp"

namespace mhe {

class Light;
struct LightInstance;
struct LightData;
struct DirectionalLightData;

const size_t gbuffer_depth_render_target_index = 3;

class AbstractGBufferFillMaterialSystem : public MaterialSystem
{
public:
    virtual RenderTarget::IdType render_target() const = 0;
};

class AbstractGBufferUseMaterialSystem : public MaterialSystem
{
public:
    virtual void set_render_target(Context& context, RenderTarget::IdType render_target) = 0;
    virtual const TextureInstance& lighting_texture() const = 0;
};

struct GBuffer
{
    TextureInstance albedo;
    TextureInstance normal;
    TextureInstance accumulator;
    TextureInstance depth;

    TextureInstance lighting;
};

class MHE_EXPORT GBufferFillMaterialSystem : public AbstractGBufferFillMaterialSystem
{
    SETUP_MATERIAL("gbuffer_fill");
public:
    GBufferFillMaterialSystem();
    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void destroy(Context& context) override;

    void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;

    RenderTarget::IdType render_target() const
    {
        return render_target_;
    }

    RenderTarget::IdType render_target_id() const override
    {
        return render_target_;
    }

    void output(Context& context, size_t unit, TextureInstance& texture) const override;

    void enable_baked_lighting( bool enable )
    {
        use_baked_lighting_ = enable;
    }

    void enable_normalmapping(bool enable)
    {
        use_normalmapping_ = enable;
    }

    void init_debug_views(Context& context) override;
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context, MeshPartInstance* parts_instances, MeshPart* parts, size_t count);
    void setup_uniforms(Material& material, Context& context, SceneContext& scene_context, const MeshPartInstance& part, const ModelContext& model_context) override;

    UniformBufferHandleType create_material_uniform(Context& context, const MaterialData& material_data);
    void update_material_data(UniformBuffer& uniform_buffer, const MaterialData& material_data);

    ClearCommand clear_command_;
    GPUProfileCommand profile_command_;
    ListOfCommands list_of_commands_;
    UniformBuffer::IdType transform_uniform_;
    RenderTargetDesc gbuffer_desc_;
    RenderTarget::IdType render_target_;
    bool use_baked_lighting_;
    bool use_normalmapping_;
};

class MHE_EXPORT GBufferDrawMaterialSystem : public AbstractGBufferUseMaterialSystem
{
    SETUP_MATERIAL("gbuffer_draw");

    static const size_t lights_per_pass = 4;
public:
    GBufferDrawMaterialSystem() :
        profile_command_("gbuffer_draw"),
        shadowmap_enabled_("shadowmap_enabled", true),
        shadowmap_quality_("shadowmap_quality", 1)
    {}

    GBufferDrawMaterialSystem(RenderTarget::IdType render_target) :
        profile_command_("gbuffer_draw"),
        render_target_(render_target),
        shadowmap_enabled_("shadowmap_enabled", true),
        shadowmap_quality_("shadowmap_quality", 1)
    {}

    void set_render_target(Context& context, RenderTarget::IdType render_target) override;

    void init_debug_views(Context& context) override;

    const TextureInstance& lighting_texture() const
    {
        return light_buffer_texture_;
    }

    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void destroy(Context& context) override;

    void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;
private:
    bool init_meshes(Context& context);
    bool init_fullscreen_quad(Context& context);
    bool init_sphere(Context& context);
    bool init_conus(Context& context);
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context);

    MeshInstance quad_mesh_;
    MeshInstance sphere_mesh_;
    MeshInstance conus_mesh_;
    ClearCommand clear_command_;
    GPUProfileCommand profile_command_;
    ListOfCommands list_of_commands_;
    RenderTarget::IdType render_target_;
    RenderTarget::IdType light_buffer_render_target_;
    TextureInstance light_buffer_texture_;
    TextureInstance albedo_texture_;
    TextureInstance normal_texture_;
    TextureInstance accumnulator_texture_;
    TextureInstance depth_texture_;
    RenderStateHandleType render_state_;

    GlobalVar<bool> shadowmap_enabled_;
    GlobalVar<size_t> shadowmap_quality_;
};

class ProbesAccumulatorMaterialSystem : public MaterialSystem
{
    SETUP_MATERIAL("probes_accumulator");
public:
    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void destroy(Context& context) override;

    void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;
    void set_gbuffer(const GBuffer& gbuffer);

    RenderTarget::IdType render_target() const
    {
        return render_target_;
    }

    void output(Context&, size_t unit, TextureInstance& texture) const override
    {
        if (unit == 0) texture = output_;
    }
private:
    bool init_fullscreen_quad(Context& context);
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    GBuffer gbuffer_;
    MeshInstance quad_mesh_;
    RenderTarget::IdType render_target_;
    TextureInstance output_;
    ClearCommand clear_command_;
};

}

#endif
