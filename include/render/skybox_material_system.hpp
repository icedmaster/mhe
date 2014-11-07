#ifndef __SKYBOX_MATERIAL_SYSTEM_HPP__
#define __SKYBOX_MATERIAL_SYSTEM_HPP__

#include "core/compiler.hpp"
#include "material_system.hpp"
#include "shader_program.hpp"
#include "render_buffer.hpp"

namespace mhe {

class MHE_EXPORT SkyboxMaterialSystem : public MaterialSystem
{
	SETUP_MATERIAL("skybox");
public:
	bool init(Context& context, const MaterialSystemContext& material_system_context) override;
	void close() override;

	void setup(Context& context, SceneContext& scene_context, NodeInstance* nodes, ModelContext* model_contexts, size_t count) override;
	void destroy(Context& context, SceneContext& scene_context, NodeInstance* nodes, size_t count) override;
	void update(Context& context, SceneContext& scene_context, RenderContext& render_context, NodeInstance* nodes, size_t count) override;
private:
	UniformBuffer::IdType transform_uniform_;
};

}

#endif
