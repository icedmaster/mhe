#ifndef __GBUFFER_MATERIAL_SYSTEM_HPP__
#define __GBUFFER_MATERIAL_SYSTEM_HPP__

#include "material_system.hpp"
#include "shader_program.hpp"
#include "render_buffer.hpp"
#include "render_target.hpp"
#include "node.hpp"
#include "commands.hpp"
#include "math/matrix.hpp"

#include "debug/rdbg.hpp"

namespace mhe {

class Light;
struct LightInstance;
struct LightData;
struct DirectionalLightData;

const size_t gbuffer_depth_render_target_index = 2;

class AbstractGBufferFillMaterialSystem : public MaterialSystem
{
public:
	virtual RenderTarget::IdType render_target() const = 0;
};

class AbstractGBufferUseMaterialSystem : public MaterialSystem
{
public:
	virtual void set_render_target(Context& context, RenderTarget::IdType render_target) = 0;
	virtual const TextureInstance& lighting_texture() const = 0;
};

class MHE_EXPORT GBufferFillMaterialSystem : public AbstractGBufferFillMaterialSystem
{
	SETUP_MATERIAL("gbuffer_fill");
public:
	GBufferFillMaterialSystem();
	bool init(Context& context, const MaterialSystemContext& material_system_context) override;
	void close() override;

	void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;

	RenderTarget::IdType render_target() const
	{
		return render_target_;
	}

	void output(Context& context, size_t unit, TextureInstance& texture) const override;
private:
	void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;
	void update(Context& context, SceneContext& scene_context, RenderContext& render_context, MeshPartInstance* parts_instances, MeshPart* parts, size_t count);
	void setup_uniforms(Material& material, Context& context, SceneContext& scene_context, const MeshPartInstance& part, const ModelContext& model_context) override;

	ClearCommand clear_command_;
	GPUProfileCommand profile_command_;
	ListOfCommands list_of_commands_;
	UniformBuffer::IdType transform_uniform_;
	RenderTargetDesc gbuffer_desc_;
	RenderTarget::IdType render_target_;
};

class MHE_EXPORT GBufferDrawMaterialSystem : public AbstractGBufferUseMaterialSystem
{
	SETUP_MATERIAL("gbuffer_draw");

	static const size_t lights_per_pass = 4;
public:
	GBufferDrawMaterialSystem() :
		profile_command_("gbuffer_draw"),
		shadowmap_enabled_("shadowmap_enabled", true),
		shadowmap_quality_("shadowmap_quality", 1)
	{}

	GBufferDrawMaterialSystem(RenderTarget::IdType render_target) :
		profile_command_("gbuffer_draw"),
		render_target_(render_target),
		shadowmap_enabled_("shadowmap_enabled", true),
		shadowmap_quality_("shadowmap_quality", 1)
	{}

	void set_render_target(Context& context, RenderTarget::IdType render_target) override;

	const TextureInstance& lighting_texture() const
	{
		return light_buffer_texture_;
	}

	bool init(Context& context, const MaterialSystemContext& material_system_context) override;
	void close() override;

	void setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) override;
private:
	bool init_meshes(Context& context);
	bool init_fullscreen_quad(Context& context);
	bool init_sphere(Context& context);
	bool init_conus(Context& context);
	void update(Context& context, SceneContext& scene_context, RenderContext& render_context);
	mat4x4 update_light_transform(const Light& light, const vec3& position, const vec3& direction) const;
		void update_light_data(LightData& light_data, const SceneContext& scene_context, const LightInstance& light_instance, const Light& light) const;
		void update_directional_light_data(DirectionalLightData& light_data, const SceneContext& scene_context, const LightInstance& light_instance, const Light& light) const;

	MeshInstance quad_mesh_;
	MeshInstance sphere_mesh_;
	MeshInstance conus_mesh_;
	ClearCommand clear_command_;
	GPUProfileCommand profile_command_;
	ListOfCommands list_of_commands_;
	RenderTarget::IdType render_target_;
	RenderTarget::IdType light_buffer_render_target_;
	TextureInstance light_buffer_texture_;
	TextureInstance albedo_texture_;
	TextureInstance normal_texture_;
	TextureInstance depth_texture_;
	UniformBuffer::IdType light_uniform_[max_lights_number];
	UniformBuffer::IdType directional_light_uniform_[max_directional_lights_number];
	DrawCallData::IdType draw_call_data_;

	GlobalVar<bool> shadowmap_enabled_;
	GlobalVar<size_t> shadowmap_quality_;
};

}

#endif
