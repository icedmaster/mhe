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
	bool init(Context& context, const MaterialSystemContext& material_system_context) override;
	void close() override;

	void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;
	void start_frame(Context&, SceneContext&, RenderContext&) override;

	RenderTarget::IdType render_target_id() const override
	{
		return render_target_id_;
	}
private:
	void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;
	void calculate_projection(mat4x4& proj, const vec4* lightspace_aabb, const mat4x4& light_view, const CameraData& camera_data, float znear, float zfar) const;

	ClearCommand clear_command_;
	fixed_size_vector<DrawCallData::IdType, max_cascades_number> draw_call_data_id_;
	fixed_size_vector<UniformBuffer::IdType, max_cascades_number> transform_uniform_id_;
	TextureInstance shadowmap_;
	RenderTarget::IdType render_target_id_;
	size_t cascades_number_;
	fixed_size_vector<float, max_cascades_number> percentage_;
	ShadowInfo shadow_info_;
};

}

#endif
