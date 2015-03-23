#ifndef __GBUFFER_HELPER_HPP__
#define __GBUFFER_HELPER_HPP__

#include "gbuffer_material_system.hpp"
#include "context.hpp"

namespace mhe {

// OBSOLETE, need to remove it
void setup_deferred_pipeline(Context& context, AbstractGBufferFillMaterialSystem* fill_material_system,
							 AbstractGBufferUseMaterialSystem* use_material_system, MaterialSystem* draw_material_system)
{
	RenderTarget& render_target = context.render_target_pool.get(fill_material_system->render_target());
	if (use_material_system != nullptr)
        use_material_system->set_render_target(context, fill_material_system->render_target());
	const TextureInstance* textures = nullptr;
	if (draw_material_system != nullptr && render_target.color_textures(&textures))
	{
		draw_material_system->set_texture(textures[0]);
		if (use_material_system)
			draw_material_system->set_texture(1, use_material_system->lighting_texture());
	}
}

}

#endif
