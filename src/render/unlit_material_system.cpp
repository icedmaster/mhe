#include "render/unlit_material_system.hpp"

#include "render/context.hpp"
#include "render/render_context.hpp"
#include "render/instances.hpp"
#include "render/layouts.hpp"
#include "render/uniforms.hpp"

namespace mhe {

bool UnlitMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	StandartGeometryLayout::init(context);
	set_layout(StandartGeometryLayout::handle);

	if (!context.shader_manager.get(shader(), material_system_context.shader_name))
		return false;
	transform_uniform_ = context.uniform_pool.create();
	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	UniformBufferDesc uniform_buffer_desc;
    uniform_buffer_desc.size = sizeof(TransformSimpleData);
    uniform_buffer_desc.unit = perframe_data_unit;
	return uniform.init(uniform_buffer_desc);
}

void UnlitMaterialSystem::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    standart_material_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void UnlitMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    DEFAULT_MATERIAL_UPDATE(context, scene_context, render_context);
}

void UnlitMaterialSystem::update(Context& context, SceneContext& /*scene_context*/, RenderContext& render_context, MeshPartInstance* /*parts*/, size_t /*count*/)
{
	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	TransformSimpleData transform_data;
	transform_data.vp = render_context.vp;
	uniform.update(transform_data);
}

void UnlitMaterialSystem::setup_uniforms(Material& material, Context& /*context*/, SceneContext& /*scene_context*/, const MeshPartInstance& /*part*/, const ModelContext& model_context)
{
	material.uniforms[0] = transform_uniform_;
    material.uniforms[1] = model_context.transform_uniform;
}

}
