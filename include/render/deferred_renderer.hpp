#ifndef __DEFERRED_RENDERER_HPP__
#define __DEFERRED_RENDERER_HPP__

#include "renderer.hpp"
#include "gbuffer_material_system.hpp"

namespace mhe {

class MHE_EXPORT DeferredRenderer : public Renderer
{
public:
	DeferredRenderer(Context& context) :
		Renderer(context)
	{}

	void init(AbstractGBufferFillMaterialSystem* fill, AbstractGBufferUseMaterialSystem* light, PosteffectMaterialSystemBase* draw);
	void enable();
	void disable();
private:
	void init_priorities();

	void render_impl(Context& context, RenderContext& render_context, SceneContext& scene_context) override;
	void debug_mode_changed(DebugMode mode, MaterialSystemId material_system_id) override;

	MaterialSystem* shadowmap_depth_write_material_system_;
	AbstractGBufferFillMaterialSystem* gbuffer_fill_material_system_;
	AbstractGBufferUseMaterialSystem* gbuffer_light_material_system_;
	PosteffectMaterialSystemBase* draw_material_system_;
};

}

#endif
