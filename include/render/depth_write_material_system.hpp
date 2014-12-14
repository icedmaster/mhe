#ifndef __DEPTH_WRITE_MATERIAL_SYSTEM_HPP__
#define __DEPTH_WRITE_MATERIAL_SYSTEM_HPP__

#include "material_system.hpp"

#include "node.hpp"
#include "commands.hpp"
#include "core/fixed_size_vector.hpp"
#include "math/matrix.hpp"

namespace mhe {

class DepthWriteMaterialSystem : public MaterialSystem
{
	SETUP_MATERIAL("depth_write");

	struct UniformData
	{
		mat4x4 mvp;
	};
public:
	bool init(Context& context, const MaterialSystemContext& material_system_context) override;
	void close() override;

	void setup(Context& context, SceneContext& scene_context, NodeInstance* nodes, ModelContext* model_contexts, size_t count) override;
	void destroy(Context& context, SceneContext& scene_context, NodeInstance* nodes, size_t count) override;
	void update(Context& context, SceneContext& scene_context, RenderContext& render_context, NodeInstance* nodes, size_t count) override;

	bool prepare_render_pass(RenderPass& pass, Context& context, SceneContext& scene_context, RenderContext& render_context) override;
private:
	bool init_draw_call_data(Context& context);

	fixed_size_vector<Node, max_shadowmap_nodes_number> nodes_;
	ClearCommand clear_command_;
	UniformBuffer::IdType uniform_;
	DrawCallData::IdType draw_call_data_;
	TextureInstance shadowmap_;
};

}

#endif