#ifndef __VCT_MATERIAL_SYSTEM_HPP__
#define __VCT_MATERIAL_SYSTEM_HPP__

#include "material_system.hpp"
#include "render_common.hpp"
#include "math/vector3.hpp"
#include "res/serialize.hpp"
#include "res/serialize_basic_types.hpp"

namespace mhe {

namespace res
{
    struct VCTSettings : public Serializable
    {
        static const size_t default_size = 512;
        uivec3 size;

        bool write(Serializer& serializer) const override
        {
            WRITE_BASIC_TYPE_FIELD(size, serializer);
            return true;
        }

        bool read(Deserializer& serializer) override
        {
            READ_BASIC_TYPE_FIELD_WITH_DEFAULT(size, serializer, uivec3(default_size, default_size, default_size));
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

    size_t default_instances_number() const override
    {
        return 1;
    }
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    Settings settings_;
    TextureBufferHandleType position_texture_id_;
    TextureHandleType color_texture_id_;
    TextureHandleType normal_texture_id_;
};

}

#endif
