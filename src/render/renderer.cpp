#include "render/renderer.hpp"

#include "render/context.hpp"
#include "render/render_context.hpp"
#include "render/uniforms.hpp"
#include "render/instances.hpp"
#include "render/scene_context.hpp"
#include "render/material_system.hpp"
#include "debug/profiler.hpp"

namespace mhe {

namespace
{
    struct DrawCallSortHelper
    {
        DrawCallSortHelper(const Context& context) : context_(context) {}
        bool operator() (const DrawCall& dc1, const DrawCall& dc2) const
        {
            return context_.material_systems.get(dc1.material.material_system)->priority() <
                   context_.material_systems.get(dc2.material.material_system)->priority();
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
    ModelContext model_context;
    model_context.color_textures[0] = albedo_texture_name;
    model_context.normal_texture = normalmap_texture_name;
    model_context.transform_uniform = node.mesh.shared_uniform;
    material_system->setup(context, scene_context, &node.mesh.instance_parts[0], &node.mesh.mesh.parts[0], &model_context, 1);
}

void Renderer::update(RenderContext& render_context, SceneContext& scene_context)
{
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

}
