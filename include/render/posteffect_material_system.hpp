#ifndef __POSTEFFECT_MATERIAL_SYSTEM_HPP__
#define __POSTEFFECT_MATERIAL_SYSTEM_HPP__

#include "core/array.hpp"
#include "material_system.hpp"
#include "material.hpp"
#include "shader_program.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "commands.hpp"

namespace mhe {

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

	void set_viewports_number(size_t number)
	{
		textures_number_ = number;
	}
private:
	bool init_mesh(Context& context, const MaterialSystemContext& material_system_context);
	bool init_mesh_instance(Context& context, MeshInstance& mesh_instance, const rect<int>& viewport, bool overdraw);

	void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

	ClearCommand clear_command_;
	MeshInstance mesh_[max_textures_number];
	TextureInstance textures_[max_textures_number];
	MeshInstance fullscreen_mesh_;
	TextureInstance fullscreen_texture_;
	uint8_t texture_type_mask_;
	uint8_t textures_number_;
};

class PosteffectMaterialSystemBase : public MaterialSystem
{
	static const size_t max_textures_number = 4;
	static const size_t invalid_index = static_cast<size_t>(-1);
public:
	PosteffectMaterialSystemBase();

	bool init(Context& context, const MaterialSystemContext& material_system_context) override;
	void close() override;

	void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;

	size_t default_instances_number() const override
	{
		return 1;
	}

	size_t inputs_number() const
	{
		return inputs_number_;
	}

	size_t outputs_number() const
	{
		return outputs_number_;
	}

	TextureInstance& input(size_t index)
	{
		return inputs_[index];
	}

	TextureInstance& output(size_t index)
	{
		return outputs_[index];
	}

	void set_input(size_t index, const TextureInstance& texture)
	{
		inputs_[index] = texture;
		inputs_number_ = max(inputs_number_, index) + 1;
	}

	void set_output(size_t index, const TextureInstance& texture)
	{
		outputs_[index] = texture;
		outputs_number_ = max(outputs_number_, index) + 1;
	}

	void set_inputs_number(size_t inputs)
	{
		inputs_number_ = inputs;
	}

	void set_outputs_number(size_t outputs)
	{
		outputs_number_ = outputs;
	}

	void set_inputs(size_t number, const TextureInstance* textures)
	{
		for (size_t i = 0; i < number; ++i)
			inputs_[i] = textures[i];
		inputs_number_ = number;
	}

	void set_outputs(size_t number, const TextureInstance* textures)
	{
		for (size_t i = 0; i < number; ++i)
			outputs_[i] = textures[i];
		outputs_number_ = number;
	}

	bool init_screen_input(Context& context, size_t index, uint8_t render_stage = render_stage_before_render_target_setup);
	bool create_output(Context& context, size_t index, float scale);
protected:
	MeshInstance& mesh_instance()
	{
		return mesh_;
	}

	void fill_render_target_desc(RenderTargetDesc& desc) const;

	virtual bool create_output(DrawCallData& draw_call_data, Context& context, size_t index, float scale);
	void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;
private:
	bool init_mesh(Context& context, const MaterialSystemContext& material_system_context);

	array<TextureInstance, max_textures_number> inputs_;
	array<TextureInstance, max_textures_number> outputs_;
	size_t inputs_number_;
	size_t outputs_number_;
	MeshInstance mesh_;
	ClearCommand clear_command_;
	CopyFramebufferCommand copy_framebuffer_command_;
	ListOfCommands list_of_commands_;
	size_t framebuffer_input_;
	UberShader::Info inputs_number_shader_info_;
};

class PosteffectSimpleMaterialSystem : public PosteffectMaterialSystemBase
{
	SETUP_MATERIAL("posteffect_simple");
};

class TonemapMaterialSystem : public PosteffectMaterialSystemBase
{
	SETUP_MATERIAL("tonemap");
};

class FXAAMaterialSystem : public PosteffectMaterialSystemBase
{
	SETUP_MATERIAL("fxaa");
};

class SSRMaterialSystem : public PosteffectMaterialSystemBase
{
	SETUP_MATERIAL("ssr");
};

class BlurMaterialSystem : public PosteffectMaterialSystemBase
{
	SETUP_MATERIAL("blur");
public:
	bool init(Context& context, const MaterialSystemContext& material_system_context) override;

	size_t default_instances_number() const override
	{
		return 2;
	}
private:
	bool create_output(DrawCallData& draw_call_data, Context& context, size_t index, float scale) override;
	void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

	UberShader::Info pass_info_;
	MeshInstance second_pass_mesh_;
	ClearCommand clear_command_;
};

class DOFMaterialSystem : public PosteffectMaterialSystemBase
{
	SETUP_MATERIAL("dof");
};

}

#endif
