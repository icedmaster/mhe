#include "render/cubemap_creation_material_system.hpp"

#include "render/context.hpp"
#include "render/scene_context.hpp"
#include "render/render_context.hpp"
#include "render/draw_call.hpp"

namespace mhe {

bool CubemapCreationMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    if (!context.shader_manager.get(shader(), material_system_context.shader_name))
    {
        ERROR_LOG("Can't get shader:" << material_system_context.shader_name);
        return false;
    }

	if (!init_uniforms(context))
		return false;

	return init_texture(context);
}

bool CubemapCreationMaterialSystem::init_uniforms(Context& context)
{
	UniformBufferDesc uniform_buffer_desc;
	uniform_buffer_desc.unit = 0;
	uniform_buffer_desc.update_type = uniform_buffer_normal;
	uniform_buffer_desc.size = sizeof(PerSideData);
	for (int i = 0; i < 6; ++i)
	{
		UniformBuffer& uniform_buffer = create_and_get(context.uniform_pool);
		if (!uniform_buffer.init(uniform_buffer_desc))
		{
			ERROR_LOG("Couldn't initialize a uniform buffer");
			return false;
		}
		uniforms_[i] = uniform_buffer.id();
	}
	return true;
}

bool CubemapCreationMaterialSystem::init_texture(Context& context)
{
	draw_call_data_id_ = context.draw_call_data_pool.create();
	DrawCallData& draw_call_data = context.draw_call_data_pool.get(draw_call_data_id_);
	RenderState& render_state = create_and_get(context.render_state_pool);
	RenderStateDesc render_state_desc;
	if (!render_state.init(render_state_desc))
	{
		ERROR_LOG("Couldn't initialize a render state");
		return false;
	}
	RenderTargetDesc render_target_desc;
	render_target_desc.width = cubemap_default_texture_size;
	render_target_desc.height = cubemap_default_texture_size;
	render_target_desc.depth_datatype = format_float;
	render_target_desc.depth_format = format_d24f;
	render_target_desc.use_depth = true;
	render_target_desc.use_stencil = false;
	render_target_desc.target = rt_readwrite;
	render_target_desc.texture_type = texture_cube;
	render_target_desc.color_targets = 1;
	render_target_desc.color_format[0] = format_rgba;
	render_target_desc.color_datatype[0] = format_ubyte;
	RenderTarget& render_target = create_and_get(context.render_target_pool);
	if (!render_target.init(context, render_target_desc))
	{
		ERROR_LOG("Couldn't initialize a render target");
		return false;
	}
	render_target.color_texture(texture_, 0);
	return true;
}

void CubemapCreationMaterialSystem::close()
{}

void CubemapCreationMaterialSystem::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
	ASSERT(0, "This method shouldn't be called");
	empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void CubemapCreationMaterialSystem::update(Context& /*context*/, SceneContext& /*scene_context*/, RenderContext& /*render_context*/)
{}

void CubemapCreationMaterialSystem::render_cubemap(Context& context, SceneContext& scene_context, RenderContext& render_context, const vec3& position, float size)
{
	NodeInstance* nodes = scene_context.node_pool.all_objects();
	for (size_t i = 0; i < scene_context.node_pool.size(); ++i)
	{
		MeshInstance& mesh_instance = nodes[i].mesh;
		Mesh& mesh = mesh_instance.mesh;
		for (size_t j = 0; j < mesh_instance.instance_parts.size(); ++j)
		{
			DrawCall& draw_call = render_context.draw_calls.add();
			draw_call.draw_call_data = draw_call_data_id_;
			draw_call.render_data = mesh.parts[i].render_data;
		}
	}

	quatf rotations[6] = {quatf(0.0f, pi_2, 0.0f), quatf(0.0f, -pi_2, 0.0f),
		quatf(pi_2, 0.0f, 0.0f ), quatf(-pi_2, 0.0f, 0.0f), quatf(0.0f, 0.0f, 0.0f), quatf(0.0f, pi, 0.0f) };
	mat4x4 view;
	view.set_lookAt(position, position + vec3(0.0f, 0.0f, -1.0f), vec3::up());
	mat4x4 proj;
	float side = cubemap_default_texture_size / 2;
	proj.set_ortho(-side, side, -side, side, 0.1f, side);
	mat4x4 transform = view * proj;
	for (int i = 0; i < 6; ++i)
	{
		UniformBuffer& uniform_buffer = context.uniform_pool.get(uniforms_[i]);
		PerSideData perside_data;
		perside_data.side_vp = transform * rotations[i].to_matrix<mat4x4>();
		uniform_buffer.update(perside_data);
	}
}

}
