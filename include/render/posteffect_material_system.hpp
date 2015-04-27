#ifndef __POSTEFFECT_MATERIAL_SYSTEM_HPP__
#define __POSTEFFECT_MATERIAL_SYSTEM_HPP__

#include "material_system.hpp"
#include "material.hpp"
#include "shader_program.hpp"
#include "texture.hpp"
#include "mesh.hpp"

namespace mhe {

class PosteffectSimpleMaterialSystem : public MaterialSystem
{
	SETUP_MATERIAL("posteffect_simple");
private:
	static const size_t max_textures_number = 4;
public:
	bool init(Context& context, const MaterialSystemContext& material_system_context) override;
	void close() override;

    void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;

	void set_texture(const TextureInstance& texture)
	{
		textures_[0] = texture;
	}

	void set_texture(size_t unit, const TextureInstance& texture)
	{
		ASSERT(unit < max_textures_number, "Invalid unit for texture");
		textures_[unit] = texture;
	}
private:
    bool init_mesh(Context& context, const MaterialSystemContext& material_system_context);

    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

  MeshInstance mesh_;
	TextureInstance textures_[max_textures_number];
};

class PosteffectDebugMaterialSystem : public MaterialSystem
{
	SETUP_MATERIAL("posteffect_debug");
private:
	static const size_t max_textures_number = 4;
public:
	bool init(Context& context, const MaterialSystemContext& material_system_context) override;
	void close() override;

	void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;

	void set_texture(size_t unit, const TextureInstance& texture)
	{
		ASSERT(unit < max_textures_number, "Invalid unit for texture");
		textures_[unit] = texture;
	}

	void set_render_target(const RenderTarget& render_target);
private:
	bool init_mesh(Context& context, const MaterialSystemContext& material_system_context);

	void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

	MeshInstance mesh_[max_textures_number];
	TextureInstance textures_[max_textures_number];
	uint8_t texture_type_mask_;
};

}

#endif
