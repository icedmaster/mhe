#include "render/unlit_material_system.hpp"

#include "render/context.hpp"
#include "render/render_context.hpp"
#include "render/node.hpp"
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
	create_uniform_buffer_element(uniform_buffer_desc, "vp", mat4x4::identity());
	uniform_buffer_desc.name = "transform";
	uniform_buffer_desc.program = &default_program(context);
	return uniform.init(uniform_buffer_desc);
}

void UnlitMaterialSystem::setup(Context& context, Node* nodes, ModelContext* model_contexts, size_t count)
{
	standart_material_setup(context, nodes, model_contexts, count, 1);
}

void UnlitMaterialSystem::destroy(Context& context, Node* nodes, size_t count)
{
	if (!count) return;
	ASSERT(nodes, "Invalid nodes");

	for (size_t i = 0; i < count; ++i)
	{
		if (context.materials[id()].is_valid(nodes[i].main_pass.material.id))
			context.materials[id()].remove(nodes[i].main_pass.material.id);
	}
}

void UnlitMaterialSystem::update(Context& context, RenderContext& render_context, Node* nodes, Transform* transforms, size_t* indexes, size_t count)
{
	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	TransformSimpleData transform_data;
	transform_data.vp = render_context.vp;
	uniform.update(transform_data);

	PerModelSimpleData permodel;
	for (size_t i = 0; i < count; ++i)
	{
		Material& material = context.materials[id()].get(nodes[i].main_pass.material.id);
		UniformBuffer& uniform = context.uniform_pool.get(material.uniforms[1]);
		permodel.model = transform(nodes[i], transforms, indexes).transform();
		uniform.update(permodel);
	}
}

void UnlitMaterialSystem::setup_uniforms(Material& material, Context& context, const ModelContext& model_context)
{
	material.uniforms[0] = transform_uniform_;
	UniformBuffer::IdType id = context.uniform_pool.create();
	material.uniforms[1] = id;
	UniformBuffer& uniform = context.uniform_pool.get(id);
	UniformBufferDesc uniform_buffer_desc;
	create_uniform_buffer_element(uniform_buffer_desc, "model", model_context.model);
	uniform_buffer_desc.name = "permodel";
	uniform_buffer_desc.program = &default_program(context);
	uniform.init(uniform_buffer_desc);
}

}
