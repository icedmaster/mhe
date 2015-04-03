#include "render/skybox_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/render_context.hpp"
#include "render/instances.hpp"
#include "render/utils/simple_meshes.hpp"

namespace mhe {

bool SkyboxMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	set_layout(SkyboxLayout::handle);

	if (!context.shader_manager.get(shader(), material_system_context.shader_name))
		return false;
	transform_uniform_ = context.uniform_pool.create();
	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	UniformBufferDesc uniform_buffer_desc;
	create_uniform_buffer_element(uniform_buffer_desc, "inv_vp", mat4x4::identity());
	uniform_buffer_desc.name = "transform";
	uniform_buffer_desc.program = &default_program(context);
    if (!uniform.init(uniform_buffer_desc))
    {
        ERROR_LOG("Skybox material initialization failed - can't init uniform");
        return false;
    }
    return init_mesh(context, material_system_context);
}

bool SkyboxMaterialSystem::init_mesh(Context& context, const MaterialSystemContext& material_system_context)
{
    skybox_mesh_.mesh.parts.resize(1);
    skybox_mesh_.instance_parts.resize(1);
    if (!utils::create_skybox_quad(skybox_mesh_, context))
    {
        ERROR_LOG("Can't create skybox mesh");
        return false;
    }

    const std::string& texture_name = material_system_context.defs[0];
    if (texture_name.empty())
    {
        ERROR_LOG("Can't initialize SkyboxMaterial - texture filename is empty");
        return false;
    }

    TextureInstance skybox_texture;
    if (!context.texture_manager.get(skybox_texture, texture_name))
    {
        ERROR_LOG("Can't get skybox texture with name:" << texture_name);
        return false;
    }

    skybox_mesh_.instance_parts[0].material.material_system = id();
    skybox_mesh_.instance_parts[0].material.id = context.materials[id()].create();
    Material& material = context.materials[id()].get(skybox_mesh_.instance_parts[0].material.id);
    material.shader_program = ubershader(context).get_default();
    material.uniforms[0] = transform_uniform_;
    skybox_mesh_.mesh.parts[0].render_data.layout = SkyboxLayout::handle;

    RenderStateDesc render_state_desc;
    render_state_desc.stencil.enabled = false;
    render_state_desc.depth.enabled = false;
    skybox_mesh_.instance_parts[0].draw_call_data = context.draw_call_data_pool.create();
    DrawCallData& draw_call_data = context.draw_call_data_pool.get(skybox_mesh_.instance_parts[0].draw_call_data);
    draw_call_data.state = context.render_state_pool.create();
    context.render_state_pool.get(draw_call_data.state).init(render_state_desc);
    return true;
}

void SkyboxMaterialSystem::close()
{
}

void SkyboxMaterialSystem::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void SkyboxMaterialSystem::update(Context& context, SceneContext& /*scene_context*/, RenderContext& render_context)
{
	mat4x4 inv_vp = render_context.vp;
	inv_vp.set_row(3, vec4(0, 0, 0, 1));
	inv_vp.inverse();

	UniformBuffer& uniform = context.uniform_pool.get(transform_uniform_);
	UniformBufferDesc uniform_buffer_desc;
	create_uniform_buffer_element(uniform_buffer_desc, "inv_vp", inv_vp);
	uniform_buffer_desc.name = "transform";
	uniform.update(uniform_buffer_desc);

    setup_draw_call(render_context.draw_calls.add(), skybox_mesh_.instance_parts[0], skybox_mesh_.mesh.parts[0]);
}

}
