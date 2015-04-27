#ifndef __CSM_DEPTH_RENDERING_MATERIAL_SYSTEM_HPP__
#define __CSM_DEPTH_RENDERING_MATERIAL_SYSTEM_HPP__

#include "material_system.hpp"
#include "commands.hpp"

namespace mhe {

class CSMDepthRenderingMaterialSystem : public MaterialSystem
{
	SETUP_MATERIAL("csm_depth_write");
public:
	bool init(Context& context, const MaterialSystemContext& material_system_context) override;
	void close() override;

    void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

	ClearCommand clear_command_;
};

}

#endif
