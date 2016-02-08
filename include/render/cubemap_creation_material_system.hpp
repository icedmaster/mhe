#ifndef __CUBEMAP_CREATION_MATERIAL_SYSTEM_HPP__
#define __CUBEMAP_CREATION_MATERIAL_SYSTEM_HPP__

#include "math/matrix.hpp"
#include "material_system.hpp"
#include "texture.hpp"
#include "node.hpp"
#include "commands.hpp"

namespace mhe {

class MHE_EXPORT CubemapCreationMaterialSystem : public MaterialSystem
{
    SETUP_MATERIAL("cubemap_creation");
public:
    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void destroy(Context&) override;

    void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;
    const TextureInstance& texture() const
    {
        return texture_;
    }

    void render_cubemap(Context& context, SceneContext& scene_context, RenderContext& render_context, const vec3& position, float size);
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

    bool init_uniforms(Context& context);
    bool init_texture(Context& context);

    UniformBuffer::IdType uniforms_[6];
    TextureInstance texture_;
    RenderStateHandleType render_state_;
    RenderTargetHandleType render_target_;
    ClearCommand command_;
};

}

#endif
