#ifndef __GBUFFER_MATERIAL_SYSTEM_HPP__
#define __GBUFFER_MATERIAL_SYSTEM_HPP__

#include "material_system.hpp"
#include "shader_program.hpp"
#include "render_buffer.hpp"
#include "render_target.hpp"
#include "node.hpp"
#include "commands.hpp"

#include "debug/rdbg.hpp"

namespace mhe {

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
private:
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context) override;
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context, MeshPartInstance* parts, size_t count);
    void setup_uniforms(Material& material, Context& context, SceneContext& scene_context, const MeshPartInstance& part, const ModelContext& model_context) override;

	ClearCommand clear_command_;
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
		shadowmap_enabled_("shadowmap_enabled", true),
		shadowmap_quality_("shadowmap_quality", 1)
	{}

	GBufferDrawMaterialSystem(RenderTarget::IdType render_target) :
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
    bool init_mesh(Context& context);
    void update(Context& context, SceneContext& scene_context, RenderContext& render_context);
	size_t calculate_passes_number(RenderContext& render_context, size_t* passes) const;

    MeshInstance mesh_;
	ClearCommand clear_command_;
	RenderTarget::IdType render_target_;
	RenderTarget::IdType light_buffer_render_target_;
	TextureInstance light_buffer_texture_;
    TextureInstance normal_texture_;
    TextureInstance depth_texture_;
	UniformBuffer::IdType light_uniform_[max_lights_number];
	UniformBuffer::IdType transform_uniform_;
    DrawCallData::IdType draw_call_data_;
	size_t lights_per_pass_;

	GlobalVar<bool> shadowmap_enabled_;
	GlobalVar<size_t> shadowmap_quality_;
};

}

#endif
