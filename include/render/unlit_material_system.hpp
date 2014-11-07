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
	void close() override {}

	void update(Context& context, SceneContext& scene_context, RenderContext& render_context, NodeInstance* nodes, size_t count) override;
	void setup(Context& context, SceneContext& scene_context, NodeInstance* materials, ModelContext* model_contexts, size_t count) override;
	void destroy(Context& context, SceneContext& scene_context, NodeInstance* nodes, size_t count) override;
private:
	void setup_uniforms(Material& material, Context& context, SceneContext& scene_context, const NodeInstance& node, const ModelContext& model_context) override;

	UniformBuffer::IdType transform_uniform_;
};

}

#endif
