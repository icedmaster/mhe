#ifndef __VCT_MATERIAL_SYSTEM_HPP__
#define __VCT_MATERIAL_SYSTEM_HPP__

#include "material_system.hpp"
#include "render_common.hpp"
#include "atomics.hpp"
#include "commands.hpp"
#include "math/vector3.hpp"
#include "math/matrix.hpp"
#include "res/serialize.hpp"
#include "res/serialize_basic_types.hpp"

namespace mhe {

namespace res
{
    struct VCTSettings : public Serializable
    {
        static const size_t default_size = 64;
        uivec3 size;
        float cell_size;

        VCTSettings() : size(default_size, default_size, default_size), cell_size(1.0f) {}

        bool write(Serializer& serializer) const override
        {
            WRITE_BASIC_TYPE_FIELD(size, serializer);
            WRITE_FIELD(cell_size, serializer);
            return true;
        }

        bool read(Deserializer& serializer) override
        {
            TRY_READ_BASIC_TYPE_FIELD(size, serializer);
            TRY_READ_FIELD(cell_size, serializer);
            return true;
        }
    };
}

struct VCTShared
{
    res::VCTSettings dbsettings;
};

class VoxelizeMaterialSystem : public MaterialSystem
{
    SETUP_MATERIAL("vct_voxelize");
public:
    struct Settings
    {
        VCTShared shared;
    };

    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void destroy(Context& context) override;

    void setup(Context &context, SceneContext &scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;
private:
    struct UniformData
    {
        vec4 grid_size;
        mat4x4 vp[3];
        mat4x4 worldspace_to_voxelspace;
    };
    void init_uniform_data(UniformData& data) const;

    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    Settings settings_;
    TextureHandleType position_texture_id_;
    TextureHandleType color_texture_id_;
    TextureHandleType normal_texture_id_;
    UniformBufferHandleType uniform_id_;
    RenderStateHandleType render_state_id_;
    Atomic atomic_counter_;
    // clear pass
    ShaderProgramHandleType clear_shader_id_;
    ComputeCallCommand clear_draw_call_command_;
};

}

#endif
