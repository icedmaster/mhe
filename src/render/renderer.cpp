#include "render/renderer.hpp"

#include "render/context.hpp"
#include "render/render_context.hpp"
#include "render/uniforms.hpp"
#include "render/instances.hpp"
#include "render/scene_context.hpp"
#include "render/material_system.hpp"
#include "render/layouts.hpp"
#include "debug/profiler.hpp"
#include "res/resource_manager.hpp"

namespace mhe {

namespace
{
    struct DrawCallSortHelper
    {
        DrawCallSortHelper(const Context& context) : context_(context) {}
        bool operator() (const DrawCall& dc1, const DrawCall& dc2) const
        {
			MaterialSystem* ms1 = context_.material_systems.get(dc1.material.material_system);
			MaterialSystem* ms2 = context_.material_systems.get(dc2.material.material_system);
			if (ms1->priority() == ms2->priority())
				return dc1.pass < dc2.pass;
            return ms1->priority() < ms2->priority();
        }

        const Context& context_;
    };
}

bool init_node(NodeInstance& node, Context& context)
{
    ASSERT(node.mesh.shared_uniform == UniformBuffer::invalid_id, "Try to initialize already initialized node");
    UniformBuffer& uniform = create_and_get(context.uniform_pool);
    UniformBufferDesc desc;
    desc.unit = permodel_data_unit;
    desc.size = sizeof(PerModelData);
    desc.update_type = uniform_buffer_normal;
    if (!uniform.init(desc))
    {
        ERROR_LOG("Can't initialize a UniformBuffer for node");
        return false;
    }
    node.mesh.shared_uniform = uniform.id();
    return true;
}

void update_nodes(Context& context, RenderContext& render_context, SceneContext& scene_context)
{
    for (size_t i = 0; i < render_context.nodes_number; ++i)
    {
        MeshInstance& mesh = render_context.nodes[i].mesh;
        ASSERT(mesh.shared_uniform != UniformBuffer::invalid_id, "Transformation uniform has not been set");
        Transform& transform = scene_context.transform_pool.get(render_context.nodes[i].transform_id).transform;
        UniformBuffer& uniform = context.uniform_pool.get(mesh.shared_uniform);

        PerModelData data;
        data.model = transform.transform();
        data.normal = data.model;
        uniform.update(data);
    }
}

void sort_draw_calls(const Context& context, RenderContext& render_context)
{
    DrawCallSortHelper helper(context);
    DrawCall* first = render_context.draw_calls.data();
    DrawCall* last = first + render_context.draw_calls.size();
    std::sort(first, last, helper);
}

void setup_node(NodeInstance& node, MaterialSystem* material_system, Context& context, SceneContext& scene_context,
                const string& albedo_texture_name, const string& normalmap_texture_name)
{
	MaterialInitializationData initialization_data;
	initialization_data.name = "mat_" + albedo_texture_name + normalmap_texture_name;
	initialization_data.textures[albedo_texture_unit] = albedo_texture_name;
	initialization_data.textures[normal_texture_unit] = normalmap_texture_name;
	initialization_data.render_data.specular_shininess = 50.0f;

	node.mesh.mesh.parts[0].material_id = context.material_manager.get(initialization_data);
	ModelContext model_context;
	model_context.transform_uniform = node.mesh.shared_uniform;
	material_system->setup(context, scene_context, &node.mesh.instance_parts[0], &node.mesh.mesh.parts[0], &model_context, 1);
}

Renderer::Renderer(Context& context) :
    context_(context),
    skybox_material_system_(nullptr),
    shadowmap_depth_write_material_system_(nullptr),
    transparent_objects_material_system_(nullptr),
    particles_material_system_(nullptr),
		ambient_color_(0.1f, 0.1f, 0.1f, 0.1f)
{}

void Renderer::update(RenderContext& render_context, SceneContext& scene_context)
{
    PerCameraData data;
    data.vp = render_context.vp;
    data.inv_vp = render_context.inv_vp;
    data.inv_proj = render_context.proj.inverted();
    data.viewpos = vec4(render_context.viewpos, 0.0f);

	// TODO:
	data.ambient = ambient_color_;

    if (render_context.percamera_uniform == UniformBuffer::invalid_id)
    {
        UniformBuffer& buffer = create_and_get(context_.uniform_pool);
        render_context.percamera_uniform = buffer.id();
        UniformBufferDesc desc;
        desc.unit = perframe_data_unit;
        desc.size = sizeof(PerCameraData);
        buffer.init(desc);
    }
    UniformBuffer& buffer = context_.uniform_pool.get(render_context.percamera_uniform);
    buffer.update(data);

	update_nodes(context_, render_context, scene_context);
	update_impl(context_, render_context, scene_context);
}

void Renderer::render(RenderContext& render_context, SceneContext& scene_context)
{
	if (skybox_material_system_ != nullptr)
		skybox_material_system_->setup_draw_calls(context_, scene_context, render_context);
	if (shadowmap_depth_write_material_system_ != nullptr)
		shadowmap_depth_write_material_system_->setup_draw_calls(context_, scene_context, render_context);
	render_impl(context_, render_context, scene_context);
	sort_draw_calls(context_, render_context);
    execute_render(render_context);
}

void Renderer::execute_render(RenderContext& render_context)
{
	ProfilerElement pe("engine.render");
	context_.driver.begin_render();
	context_.driver.clear_color();
	context_.driver.clear_depth();

    context_.driver.render(context_, render_context.draw_calls.data(), render_context.draw_calls.size());
}

void Renderer::flush()
{
	{
		ProfilerElement end_render_pe("driver.end_render");
		context_.driver.end_render();
	}
	{
		ProfilerElement swap_buffers_pe("window_system.swap_buffers");
		context_.window_system.swap_buffers();
	}
}

void Renderer::set_skybox_material_system(MaterialSystem* material_system)
{
    skybox_material_system_ = material_system;
    skybox_material_system_->set_priority(skybox_material_system_priority);
}

void Renderer::set_shadowmap_depth_write_material_system(MaterialSystem* material_system)
{
    shadowmap_depth_write_material_system_ = material_system;
    shadowmap_depth_write_material_system_->set_priority(shadowmap_depth_write_material_system_priority);
}

bool load_node(NodeInstance& node, const string& name, hash_type material_system_name, Context& context, SceneContext& scene_context)
{
	if (!context.mesh_manager.get_instance(node.mesh, name))
	{
		ERROR_LOG("load_node() failed: can not get mesh:" << name);
		return false;
	}
	MaterialSystem* material_system = context.material_systems.get(material_system_name);
	if (material_system == nullptr)
		return false;

	std::vector<ModelContext> model_contexts(node.mesh.instance_parts.size());
	for (size_t i = 0; i < node.mesh.instance_parts.size(); ++i)
	{
		ModelContext& model_context = model_contexts[i];
		model_context.transform_uniform = node.mesh.shared_uniform;
	}
	material_system->setup(context, scene_context, &node.mesh.instance_parts[0], &node.mesh.mesh.parts[0], &model_contexts[0], node.mesh.instance_parts.size());
	return true;
}

bool init_render(Context& context)
{
	init_standart_layouts(context);

	return true;
}

}
