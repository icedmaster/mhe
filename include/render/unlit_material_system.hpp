#ifndef __UNLIT_MATERIAL_SYSTEM_HPP__
#define __UNLIT_MATERIAL_SYSTEM_HPP__

#include "material_system.hpp"
#include "shader_program.hpp"
#include "render_buffer.hpp"
#include "math/matrix.hpp"

namespace mhe {

class MHE_EXPORT UnlitMaterialSystem : public MaterialSystem
{
    SETUP_MATERIAL("unlit");
public:
    bool init(Context& context, const MaterialSystemContext& material_system_context) override;
    void destroy(Context&) override;

    void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;
    void setup_uniforms(Material& material, Context& context, SceneContext& scene_context, const MeshPartInstance& parts, const ModelContext& model_context) override;
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context, MeshPartInstance* instance_parts, MeshPart* parts, size_t count);

    UniformBuffer::IdType transform_uniform_;
};

}

#endif
