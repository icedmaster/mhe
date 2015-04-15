#ifndef __SKYBOX_MATERIAL_SYSTEM_HPP__
#define __SKYBOX_MATERIAL_SYSTEM_HPP__

#include "core/compiler.hpp"
#include "material_system.hpp"
#include "shader_program.hpp"
#include "render_buffer.hpp"
#include "node.hpp"

namespace mhe {

class MHE_EXPORT SkyboxMaterialSystem : public MaterialSystem
{
	SETUP_MATERIAL("skybox");
public:
	bool init(Context& context, const MaterialSystemContext& material_system_context) override;
	void close() override;

    void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;
    bool init_mesh(Context& context, const MaterialSystemContext& material_system_context);

    MeshInstance skybox_mesh_;
	UniformBuffer::IdType transform_uniform_;
	TextureInstance skybox_texture_;
};

}

#endif
