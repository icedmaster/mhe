#ifndef __GI_HPP__
#define __GI_HPP__

#include "material_system.hpp"
#include "render_common.hpp"
#include "mesh.hpp"
#include "core/string.hpp"

namespace mhe {

struct Context;
class ShaderStorageBuffer;
class Texture;

class CubemapIntegrator
{
public:
    struct Settings
    {
        string shader_name;
    };

    bool init(Context& context, const Settings& settings);
    bool integrate(ShaderStorageBuffer& dst, Context& context, Texture& cubemap);
private:
    UbershaderHandleType ubershader_id_;
    UniformBufferHandleType settings_uniform_id_;
};

class IndirectLightingResolveMaterialSystem : public MaterialSystem
{
    SETUP_MATERIAL("gi_resolve")
    enum
    {
        mode_constant_ambient,
        mode_sh_ambient
    };
public:
    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void destroy(Context& context) override;

    void setup(Context &context, SceneContext &scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;

    void set_global_ambient_sh_buffer(ShaderStorageBufferHandleType ssb_id)
    {
        global_ambient_sh_id_ = ssb_id;
    }

    RenderTargetHandleType resolved_diffuse_render_target_id() const
    {
        return resolved_diffuse_rt_id_;
    }

    RenderTargetHandleType resolved_specular_render_target_id() const
    {
        return resolved_specular_rt_id_;
    }

    void add_gi_diffuse(const TextureInstance& texture)
    {
        gi_diffuse_ = texture;
    }
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    bool init_fullscreen_quad(MeshInstance& mesh_instance, Context& context);

    UbershaderHandleType diffuse_resolve_ubershader_id_;
    UbershaderHandleType specular_resolve_ubershader_id_;
    RenderTargetHandleType resolved_diffuse_rt_id_;
    RenderTargetHandleType resolved_specular_rt_id_;
    MaterialHandleType diffuse_resolve_material_id_;
    MaterialHandleType specular_resolve_material_id_;
    MeshInstance diffuse_resolve_quad_mesh_;
    MeshInstance specular_resolve_quad_mesh_;

    ShaderStorageBufferHandleType global_ambient_sh_id_;

    TextureInstance gi_diffuse_;
};

}

#endif
