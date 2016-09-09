#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include "core/string.hpp"
#include "core/hash.hpp"
#include "core/ref_counter.hpp"
#include "core/fixed_size_vector.hpp"
#include "math/vector4.hpp"
#include "render_context.hpp"
#include "gi.hpp"

#include "debug/gpu_profiler.hpp"

namespace mhe {

struct Context;
struct RenderContext;
struct SceneContext;
struct NodeInstance;
struct MaterialInitializationData;

class MaterialSystem;
class PosteffectMaterialSystemBase;
class PosteffectDebugMaterialSystem;
class LPVMaterialSystem;
class RSMMaterialSystem;
class LPVResolveMaterialSystem;
class SkyboxMaterialSystem;
class IndirectLightingResolveMaterialSystem;
class Renderer;

bool init_node(NodeInstance& node, Context& context);
void update_nodes(Context& context, RenderContext& render_context, SceneContext& scene_context);
void sort_draw_calls(const Context& context, RenderContext& render_context);

MHE_EXPORT void setup_node(NodeInstance& node, MaterialSystem* material_system, Context& context, SceneContext& scene_context,
                           const FilePath& material_name,
                           const FilePath& albedo_texture_name, const FilePath& normalmap_texture_name = string());
MHE_EXPORT void setup_node(NodeInstance& node, MaterialSystem* material_system, Context& context, SceneContext& scene_context, MaterialDataIdType material_id);

MHE_EXPORT bool load_node(NodeInstance& instance, const string& name, const char* material_system_name, Context& context, SceneContext& scene_context);

template <class MaterialSystem>
bool load_node(NodeInstance& node, const string& name, Context& context, SceneContext& scene_context)
{
    return load_node(node, name, MaterialSystem::material_name(), context, scene_context);
}

class PosteffectSystem
{
public:
    struct NodeInput
    {
        size_t index;
        string node;
        size_t node_output;
        string material;
        size_t material_output;
        TextureInstance explicit_texture;
        bool copy_current_framebuffer;
    };

    struct NodeOutput
    {
        size_t index;
        float scale;
        int format;
    };

    template <class BufferHandleType>
    struct Buffer
    {
        size_t index;
        string node;
        size_t node_buffer;
        BufferHandleType explicit_handle;
        string systemwide_name;
        string material;

        Buffer() : explicit_handle(InvalidHandle<BufferHandleType>::id) {}
    };

    typedef fixed_capacity_vector<NodeInput, 8> Inputs;
    typedef fixed_capacity_vector<NodeOutput, 8> Outputs;
    typedef fixed_capacity_vector<Buffer<ShaderStorageBufferHandleType>, 4> Buffers;
    typedef fixed_capacity_vector<Buffer<UniformBufferHandleType>, 4> Uniforms;

    struct PosteffectNodeDesc
    {
        string name;
        string prev_node;
        string material;
        size_t priority;
        Inputs inputs;
        Outputs outputs;
        Buffers buffers;
        Uniforms uniforms;
        bool instantiate;
    };

    MHE_EXPORT PosteffectMaterialSystemBase* add(Context& context, const PosteffectNodeDesc& node_desc);
    MHE_EXPORT PosteffectMaterialSystemBase* create(Context& context, const PosteffectNodeDesc& node_desc);
    void process(Context& context, RenderContext& render_context, SceneContext& scene_context);
private:
    struct PosteffectNode
    {
        string name;
        PosteffectMaterialSystemBase* material_system;
    };

    const PosteffectNode* find_node(const string& name) const;
    void init_inputs(PosteffectMaterialSystemBase* material_system, Context& context, const PosteffectNodeDesc& node_desc);
    void init_outputs(PosteffectMaterialSystemBase* material_system, Context& context, const PosteffectNodeDesc& node_desc);
    void init_buffers(PosteffectMaterialSystemBase* material_system, Context& context, const PosteffectNodeDesc& node_desc);
    void init_uniforms(PosteffectMaterialSystemBase* material_system, Context& context, const PosteffectNodeDesc& node_desc);

    typedef fixed_size_vector<PosteffectNode, 16> Posteffects;
    Posteffects posteffects_;
};

class GISystem
{
public:
    struct LPVParams
    {
        size_t base_priority;
        int output_texture_format;
        float output_texture_scale;

        LPVParams() : output_texture_format(format_rgb16f), output_texture_scale(1.0f) {}
    };

    struct Settings
    {
        string diffuse_resolve_shader_name;
        string specular_resolve_shader_name;
    };

    GISystem();

    bool init(Context& context, const Settings& settings);
    void destroy(Context& context);

    void add_lpv(Context& context, Renderer& renderer, const LPVParams& params);
    void add_skybox(Context& context, const SkyboxMaterialSystem* skybox_material_system, const CubemapIntegrator::Settings& integrator_settings);

    void apply(Renderer& renderer);

    void update_skybox(Context& context);

    void before_render(Context& context, SceneContext& scene_context, RenderContext& render_context);
    void render(Context& context, SceneContext& scene_context, RenderContext& render_context);

    LPVMaterialSystem* lpv_material_system() const
    {
        return lpv_material_system_;
    }

    IndirectLightingResolveMaterialSystem* gi_resolve_material_system() const
    {
        return indirect_lighting_resolve_material_system_;
    }
private:
    RSMMaterialSystem* rsm_material_system_;
    LPVMaterialSystem* lpv_material_system_;
    LPVResolveMaterialSystem* lpv_resolve_material_system_;
    const SkyboxMaterialSystem* skybox_material_system_;

    ShaderStorageBufferHandleType ambient_sh_buffer_id_;
    CubemapIntegrator cubemap_integrator_;

    IndirectLightingResolveMaterialSystem* indirect_lighting_resolve_material_system_;
};

class MHE_EXPORT Renderer : public ref_counter
{
public:
    static const uint8_t skybox_material_system_priority = 2;
    static const uint8_t shadowmap_depth_write_material_system_priority = 3;
    static const uint8_t debug_material_system_priority = 250;

    enum DebugMode
    {
        renderer_debug_mode_none,
        renderer_debug_mode_rgba,
        renderer_debug_mode_rgb,
        renderer_debug_mode_r,
        renderer_debug_mode_g,
        renderer_debug_mode_b,
        renderer_debug_mode_a,
        renderer_debug_mode_depth
    };
public:
    struct Settings
    {
        GISystem::Settings gi_settings;
    };

    Renderer(Context& context);
    virtual ~Renderer() {}

    bool init(const Settings& settings);
    void destroy();

    virtual void before_update(SceneContext& scene_context);
    virtual void update(SceneContext& scene_context);
    virtual void render(SceneContext& scene_context);

    void set_skybox_material_system(MaterialSystem* material_system);
    void set_shadowmap_depth_write_material_system(MaterialSystem* material_system);
    void set_directional_shadowmap_depth_write_material_system(MaterialSystem* material_system);
    void set_fullscreen_debug_material_system(PosteffectMaterialSystemBase* material_system);

    void set_indirect_diffuse_lighting_texture(const TextureInstance& texture)
    {
        gi_diffuse_texture_ = texture;
    }

    const TextureInstance& indirect_diffuse_lighting_texture() const
    {
        return gi_diffuse_texture_;
    }

    void set_indirect_specular_lighting_texture(const TextureInstance& texture)
    {
        gi_specular_texture_ = texture;
    }

    const TextureInstance& indirect_specular_lighting_texture() const
    {
        return gi_specular_texture_;
    }

    void set_material_system_to_process(MaterialSystem* material_system)
    {
        material_systems_.push_back(material_system);
    }

    void set_ambient_color(const colorf& color)
    {
        ambient_color_ = color;
    }

    DebugMode debug_mode() const
    {
        return debug_mode_;
    }

    void flush();

    PosteffectSystem& posteffect_system()
    {
        return posteffect_system_;
    }

    GISystem& gi_system()
    {
        return gi_system_;
    }

    RenderContext& render_context()
    {
        return render_context_;
    }

    UniformBufferHandleType system_wide_uniform(const string& name) const;
    UniformBufferHandleType main_camera_uniform() const;

    virtual void set_gi_modifier_material_system(MaterialSystem*, size_t)
    {}

    virtual TextureInstance scene_normals_buffer() const
    {
        return TextureInstance();
    }

    virtual TextureInstance scene_depth_buffer() const
    {
        return TextureInstance();
    }

    virtual void setup_common_pass(Material& /*material*/) const
    {
    }

    void set_skybox_cubemap(const TextureInstance& cubemap);

    PosteffectMaterialSystemBase* debug_material_system() const
    {
        return fullscreen_debug_material_system_;
    }

    void set_debug_buffer(DebugMode mode, const TextureInstance& texture);
protected:
    Context& context()
    {
        return context_;
    }

    virtual void execute_render(RenderContext& render_context);
private:
    virtual void update_impl(Context& /*context*/, RenderContext& /*render_context*/, SceneContext& /*scene_context*/) {}
    virtual void render_impl(Context& context, RenderContext& render_context, SceneContext& scene_context) = 0;

    void flush_pass();

    Context& context_;
    MaterialSystem* skybox_material_system_;
    MaterialSystem* shadowmap_depth_write_material_system_;
    MaterialSystem* directional_shadowmap_depth_write_material_system_;
    // To implement
    MaterialSystem* transparent_objects_material_system_;
    MaterialSystem* particles_material_system_;

    TextureInstance gi_diffuse_texture_;
    TextureInstance gi_specular_texture_;

    PosteffectMaterialSystemBase* fullscreen_debug_material_system_;

    fixed_size_vector<MaterialSystem*, 16> material_systems_;

    colorf ambient_color_;
    DebugMode debug_mode_;

    PosteffectSystem posteffect_system_;
    GISystem gi_system_;

    RenderContext render_context_;
};

class NullRenderer : public Renderer
{
public:
    NullRenderer(Context& context) :
        Renderer(context)
        {}
private:
    void render_impl(Context& /*context*/, RenderContext& /*render_context*/, SceneContext& /*scene_context*/) override
    {}
};

bool init_render(Context& context);
void destroy_render(Context& context);

}

#endif
