#ifndef __POSTEFFECT_MATERIAL_SYSTEM_HPP__
#define __POSTEFFECT_MATERIAL_SYSTEM_HPP__

#include "material_system.hpp"
#include "material.hpp"
#include "shader_program.hpp"
#include "texture.hpp"

namespace mhe {

class PosteffectSimpleMaterialSystem : public MaterialSystem
{
	SETUP_MATERIAL("posteffect_simple");
public:
	bool init(Context& context, const MaterialSystemContext& material_system_context) override;
	void close() override;

	void setup(Context& context, SceneContext& scene_context, NodeInstance* nodes, ModelContext* model_contexts, size_t count) override;
	void destroy(Context& context, SceneContext& scene_context, NodeInstance* nodes, size_t count) override;
	void update(Context& context, SceneContext& scene_context, RenderContext& render_context, NodeInstance* nodes, size_t count) override;

	void set_texture(const TextureInstance& texture)
	{
		textures_[0] = texture;
	}

	void set_texture(size_t unit, const TextureInstance& texture)
	{
		ASSERT(unit < material_textures_number, "Invalid unit for texture");
		textures_[unit] = texture;
	}
private:
	TextureInstance textures_[material_textures_number];
};

}

#endif
