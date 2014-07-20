#include "render/unlit_material_system.hpp"

#include "render/context.hpp"
#include "render/render_context.hpp"
#include "render/node.hpp"
#include "render/layouts.hpp"

namespace mhe {

bool UnlitMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	StandartGeometryLayout::init(context);

	if (!context.shader_manager.get(shader_, material_system_context.shader_name))
		return false;
	transform_uniform_ = context.uniform_pool.create();
	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	UniformBufferDesc uniform_buffer_desc;
	create_uniform_buffer_element(uniform_buffer_desc, "vp", mat4x4::identity());
	uniform_buffer_desc.name = "transform";
	uniform_buffer_desc.program = &context.shader_pool.get(shader_.shader_program_handle);
	return uniform.init(uniform_buffer_desc);
}

void UnlitMaterialSystem::setup(Context& context, Node* nodes, ModelContext* model_contexts, size_t count)
{
	if (!count) return;
	ASSERT(nodes, "Invalid materials parameter");
	ASSERT(model_contexts, "Invalid model contexts");

	for (size_t i = 0; i < count; ++i)
	{
		Material::IdType material_id = context.materials[id()].create();
		Material& material = context.materials[id()].get(material_id);
		material.shader_program = shader_.shader_program_handle;
		setup_uniforms(material, context, model_contexts[i]);

		nodes[i].material.material_system = id();
		nodes[i].material.id = material.id;

		nodes[i].mesh.layout = StandartGeometryLayout::handle;
	}
}

void UnlitMaterialSystem::destroy(Context& context, Node* nodes, size_t count)
{
	if (!count) return;
	ASSERT(nodes, "Invalid nodes");

	for (size_t i = 0; i < count; ++i)
	{
		if (context.materials[id()].is_valid(nodes[i].material.id))
			context.materials[id()].remove(nodes[i].material.id);
	}
}

void UnlitMaterialSystem::update(Context& context, RenderContext& render_context, Node* nodes, Transform* transforms, size_t count)
{
	// update shader uniform
	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	UniformBufferDesc uniform_buffer_desc;
	create_uniform_buffer_element(uniform_buffer_desc, "vp", render_context.vp);
	uniform_buffer_desc.name = "transform";
	uniform.update(uniform_buffer_desc);

	UniformBufferDesc model_uniform_buffer_desc;
	model_uniform_buffer_desc.name = "permodel";
	for (size_t i = 0; i < count; ++i)
	{
        Material& material = context.materials[id()].get(nodes[i].material.id);
		UniformBuffer& uniform = context.uniform_pool.get(material.uniforms[1]);
        create_uniform_buffer_element(model_uniform_buffer_desc, "model", transforms[i].transform());
		uniform.update(model_uniform_buffer_desc);
		model_uniform_buffer_desc.uniforms.clear();
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
	uniform_buffer_desc.program = &context.shader_pool.get(shader_.shader_program_handle);
	uniform.init(uniform_buffer_desc);
}

}
