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

    void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context);
	bool init_light_data(Context& context);

	static const size_t default_number_of_shadowcasters = 8;

	fixed_size_vector<ClearCommand, default_number_of_shadowcasters> clear_commands_;
	fixed_size_vector<UniformBuffer::IdType, default_number_of_shadowcasters> uniforms_;
	fixed_size_vector<DrawCallData::IdType, default_number_of_shadowcasters> draw_call_data_;
	fixed_size_vector<TextureInstance, default_number_of_shadowcasters> shadowmaps_;
	fixed_size_vector<RenderState::IdType, default_number_of_shadowcasters> render_states_;
	fixed_size_vector<RenderTarget::IdType, default_number_of_shadowcasters> render_targets_;
};

}

#endif
