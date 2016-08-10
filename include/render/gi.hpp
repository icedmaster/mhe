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

    RenderTargetHandleType render_target_id() const override
    {
        return rt_id_;
    }
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    bool init_fullscreen_quad(Context& context);

    RenderTargetHandleType rt_id_;
    MeshInstance quad_mesh_;

    ShaderStorageBufferHandleType global_ambient_sh_id_;
};

}

#endif
