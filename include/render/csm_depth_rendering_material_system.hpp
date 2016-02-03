#ifndef __CSM_DEPTH_RENDERING_MATERIAL_SYSTEM_HPP__
#define __CSM_DEPTH_RENDERING_MATERIAL_SYSTEM_HPP__

#include "material_system.hpp"
#include "commands.hpp"
#include "light.hpp"
#include "math/matrix.hpp"

namespace mhe {

struct CameraData;

class CSMDepthRenderingMaterialSystem : public MaterialSystem
{
    SETUP_MATERIAL("csm_depth_write");

    static const size_t max_cascades_number = 8;
public:

    enum
    {
        frustum_extents,
        aabb_extents,
        fixed_distance,
        min_distance,
    };

    struct Settings
    {
        int distance_calculation_mode;
        float min_light_distance;

        Settings() :
            distance_calculation_mode(frustum_extents),
            min_light_distance(0.0f)
        {}
    };

    CSMDepthRenderingMaterialSystem();

    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void close() override;

    void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;
    void start_frame(Context&, SceneContext&, RenderContext&) override;

    RenderTarget::IdType render_target_id() const override
    {
        return render_target_id_;
    }

    Settings& settings()
    {
        return settings_;
    }
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;
    void calculate_projection(mat4x4& proj, mat4x4& view, const vec4* aabb, const CameraData& camera_data, float znear, float zfar, 
        const vec3& lightdir, const RenderContext& render_context) const;

    Settings settings_;
    vec2 texture_size_;
    vec2 cascade_size_;
    ClearCommand clear_command_;
    GPUProfileCommand profile_command_;
    ListOfCommands list_of_commands_;
    fixed_size_vector<RenderStateHandleType, max_cascades_number> render_states_;
    fixed_size_vector<UniformBuffer::IdType, max_cascades_number> transform_uniform_id_;
    TextureInstance shadowmap_;
    RenderTarget::IdType render_target_id_;
    size_t cascades_number_;
    fixed_size_vector<float, max_cascades_number> percentage_;
    ShadowInfo shadow_info_;
    ShaderProgram::IdType shader_program_with_skinning_;
};

}

#endif
