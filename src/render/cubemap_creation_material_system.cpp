#include "render/cubemap_creation_material_system.hpp"

#include "render/context.hpp"
#include "render/scene_context.hpp"
#include "render/render_context.hpp"
#include "render/draw_call.hpp"
#include "render/uniforms.hpp"

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
	uniform_buffer_desc.size = sizeof(PerCameraData);
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
	render_state_desc.viewport.viewport.set(0, 0, cubemap_default_texture_size, cubemap_default_texture_size);
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
	draw_call_data.render_target = render_target.id();
	draw_call_data.state = render_state.id();
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
	// TODO: remove euler angles from the constants - use quaternion values instead
	// FIXME: it looks like we have to render the +X/-X/+Z/-Z sides of the cubemap upside-down
	const quatf rotations[6] = {quatf(0.0f, -pi_2, 0.0f) * quatf(pi, 0.0f, 0.0f), quatf(0.0f, pi_2, 0.0f) * quatf(pi, 0.0f, 0.0f),
		quatf(-pi_2, 0.0f, 0.0f) * quatf(0.0f, pi, 0.0f), quatf(pi_2, 0.0f, 0.0f) * quatf(0.0f, pi, 0.0f),
		quatf(0.0f, pi, 0.0f) * quatf(pi, 0.0f, 0.0f), quatf(0.0f, 0.0f, 0.0f) * quatf(pi, 0.0f, 0.0f) };
	mat4x4 view;
	view.set_lookAt(position, position + vec3(0.0f, 0.0f, 1.0f), vec3::up());
	mat4x4 proj;
	proj.set_perspective(pi_2, 1.0f, 0.1f, size);
	for (int i = 0; i < 6; ++i)
	{
		UniformBuffer& uniform_buffer = context.uniform_pool.get(uniforms_[i]);
		PerCameraData perside_data;
		perside_data.vp = view * rotations[i].to_matrix<mat4x4>() * proj;
		uniform_buffer.update(perside_data);
	}

	command_.set_driver(&context.driver);

	NodeInstance* nodes = scene_context.node_pool.all_objects();

	DrawCallData& draw_call_data = context.draw_call_data_pool.get(draw_call_data_id_);
	RenderTarget& render_target = context.render_target_pool.get(draw_call_data.render_target);
	ASSERT(render_context.draw_calls.empty(), "The cubemap rendering should be called before the main render pass");
	for (size_t pass = 0; pass < 6; ++pass)
	{
		command_.reset();
		render_target.set_texture_side(pass);
		context.driver.reset_state();

		context.materials[id()].clear();
		for (size_t i = 0; i < scene_context.node_pool.size(); ++i)
		{
			if (!nodes[i].cast_reflection) continue;
			MeshInstance& mesh_instance = nodes[i].mesh;
			Mesh& mesh = mesh_instance.mesh;
			for (size_t j = 0; j < mesh_instance.instance_parts.size(); ++j)
			{
				DrawCall& draw_call = render_context.draw_calls.add();
				draw_call.draw_call_data = draw_call_data_id_;
				draw_call.render_data = mesh.parts[j].render_data;
				
				MaterialData material_data;
				context.material_manager.get(material_data, mesh.parts[j].material_id);
				Material& material = create_and_get(context.materials[id()]);
				material.shader_program = default_program(context).id();
				material.textures[0] = material_data.textures[albedo_texture_unit];
				material.uniforms[0] = uniforms_[pass];
				material.uniforms[1] = mesh_instance.shared_uniform;
				draw_call.material.material_system = id();
				draw_call.material.id = material.id;

				draw_call.command = &command_;
			}
		}

		context.driver.render(context, render_context.draw_calls.data(), render_context.draw_calls.size());
		render_context.draw_calls.clear();
	}
}

}
