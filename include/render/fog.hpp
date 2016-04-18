#ifndef __FOG_HPP__
#define __FOG_HPP__

#include "posteffect_material_system.hpp"
#include "render_common.hpp"

namespace mhe {

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

}

#endif
