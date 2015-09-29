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

	void output(Context&, size_t unit, TextureInstance& texture) const override
	{
		texture = outputs_[unit];
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
	bool create_output(Context& context, size_t index, float scale, int format);

	// This method will be called when PosteffectChain has initialized this material system
	virtual void postinit(Context& /*context*/) {}
protected:
	MeshInstance& mesh_instance()
	{
		return mesh_;
	}

	Material& default_material(Context& context);

	void fill_render_target_desc(RenderTargetDesc& desc, int format = format_rgba) const;

	virtual bool create_output(DrawCallData& draw_call_data, Context& context, size_t index, float scale, int format);
	void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;
	void prepare_draw_call(DrawCall& draw_call, Context& context, SceneContext& scene_context, RenderContext& render_context);
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
public:
	bool init(Context& context, const MaterialSystemContext& material_system_context) override;
	void init_debug_views(Context& context) override;
private:
	void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

	struct SSRShaderData
	{
		vec4 ssrdata[3];
	};

	SSRShaderData ssr_shader_data_;
	UniformBuffer::IdType ssr_uniform_;
};

class BlurMaterialSystem : public PosteffectMaterialSystemBase
{
	SETUP_MATERIAL("blur");

	struct ShaderData
	{
		vec4 params;
	};
public:
	struct Settings
	{
		float size;

		Settings() : size(1.0f) {}
	};

	enum
	{
		quality_normal,
		quality_high
	};

	bool init(Context& context, const MaterialSystemContext& material_system_context) override;

	size_t default_instances_number() const override
	{
		return 2;
	}

	Settings& settings()
	{
		return settings_;
	}

	void set_quality(int quality)
	{
		quality_ = quality;
	}
private:
	bool create_output(DrawCallData& draw_call_data, Context& context, size_t index, float scale, int format) override;
	void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;

	Settings settings_;
	UniformBuffer::IdType uniform_;
	UberShader::Info pass_info_;
	UberShader::Info quality_info_;
	MeshInstance second_pass_mesh_;
	ClearCommand clear_command_;
	int quality_;
};

class DOFMaterialSystem : public PosteffectMaterialSystemBase
{
	SETUP_MATERIAL("dof");

	static const size_t input_texture_unit = 3;
	static const size_t blur_texture_unit = 4;
	static const size_t dof_texture_unit = 5;

	struct DOFShaderData
	{
		vec4 planes;
		vec4 coc;
	};
public:
	bool init(Context& context, const MaterialSystemContext& material_system_context) override;

	size_t default_instances_number() const override
	{
		return 3; // blur resolve pass + DOF pass + composite pass
	}

	void postinit(Context& context) override;
	void init_debug_views(Context& context) override;
private:
	void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;
	void update_uniforms(Context& context);

	ClearCommandSimple clear_command_simple_;
	MeshInstance dof_pass_mesh_instance_;
	MeshInstance composite_pass_mesh_instance_;
	UberShader::Info pass_info_;
	float blur_resolve_pass_scale_;

	DOFShaderData dof_shader_data_;
	UniformBuffer::IdType dof_uniform_;
};

class SSAOMaterialSystem : public PosteffectMaterialSystemBase
{
	SETUP_MATERIAL("ssao");

	static const size_t noise_texture_unit = 4;
public:
	bool init(Context& context, const MaterialSystemContext& material_system_context) override;
	void init_debug_views(Context& context) override;
private:
	void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;
	void create_noise_texture(TextureInstance& texture, Context& context);

	struct SSAOShaderData
	{
		vec4 ssaodata[2];
	};

	SSAOShaderData ssao_shader_data_;
	UniformBuffer::IdType ssao_uniform_;
};

class CompositeMulMaterialSystem : public PosteffectMaterialSystemBase
{
	SETUP_MATERIAL("comp_mul");
};

class CompositeAddMaterialSystem : public PosteffectMaterialSystemBase
{
	SETUP_MATERIAL("comp_add");
};

}

#endif
