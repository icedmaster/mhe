#include "render/renderer.hpp"

#include "render/context.hpp"
#include "render/render_globals.hpp"
#include "render/uniforms.hpp"
#include "render/instances.hpp"
#include "render/scene_context.hpp"
#include "render/material_system.hpp"
#include "render/layouts.hpp"
#include "render/posteffect_material_system.hpp"
#include "render/rsm_material_system.hpp"
#include "render/lpv_material_system.hpp"
#include "render/light_instance_methods.hpp"
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
    uniform.update(PerModelData());
    node.mesh.shared_uniform = uniform.id();

    // Create texture for skinned geometry
    if (!node.mesh.mesh.skeleton.bones.empty())
    {

    }
    return true;
}

void update_nodes(Context& context, RenderContext& render_context, SceneContext& scene_context)
{
    GPU_PROFILE("update_nodes");
    for (size_t i = 0; i < render_context.nodes_number; ++i)
    {
        MeshInstance& mesh = render_context.nodes[i].mesh;
        ASSERT(mesh.shared_uniform != UniformBuffer::invalid_id, "Transformation uniform has not been set");
        Transform& transform = scene_context.transform_pool.get(render_context.nodes[i].transform_id).transform;
        UniformBuffer& uniform = context.uniform_pool.get(mesh.shared_uniform);

        PerModelData data;
        data.model = transform.transform();
        data.normal = data.model;
        data.normal.set_row(3, vec4::zero());
        uniform.update(data);
    }
}

void update_lights(Context& context, RenderContext& render_context, SceneContext& scene_context)
{
    for (size_t i = 0; i < render_context.lights_number; ++i)
        update_light_uniform(context, scene_context, render_context.lights[i]);
}

void sort_draw_calls(const Context& context, RenderContext& render_context)
{
    DrawCallSortHelper helper(context);
    DrawCall* first = render_context.draw_calls.data();
    DrawCall* last = first + render_context.draw_calls.size();
    std::sort(first, last, helper);
}

void setup_node(NodeInstance& node, MaterialSystem* material_system, Context& context, SceneContext& scene_context,
                const FilePath& material_name,
                const FilePath& albedo_texture_name, const FilePath& normalmap_texture_name)
{
    MaterialData& material_data = create_and_get(context.material_data_pool);
    material_data.render_data.glossiness = default_glossiness;
    material_data.render_data.specular_shininess = default_shininess;
    context.texture_manager.get(material_data.textures[albedo_texture_unit], albedo_texture_name);
    if (!normalmap_texture_name.empty())
        context.texture_manager.get(material_data.textures[normal_texture_unit], normalmap_texture_name);
    context.material_manager.add(material_data.id, material_name);
    setup_node(node, material_system, context, scene_context, material_data.id);
}

void setup_node(NodeInstance& node, MaterialSystem* material_system, Context& context, SceneContext& scene_context, MaterialDataIdType material_id)
{
    node.mesh.mesh.parts[0].material_id = material_id;
    ModelContext model_context;
    model_context.transform_uniform = node.mesh.shared_uniform;
    model_context.animation_texture_buffer = node.mesh.skeleton_instance.texture_buffer;
    material_system->setup(context, scene_context, &node.mesh.instance_parts[0], &node.mesh.mesh.parts[0], &model_context, 1);
}

PosteffectMaterialSystemBase* PosteffectSystem::add(Context& context, const PosteffectSystem::PosteffectNodeDesc& desc)
{
    PosteffectMaterialSystemBase* material_system = create(context, desc);
    ASSERT(material_system != nullptr, "Couldn't create a material system");
    PosteffectNode node;
    node.name = desc.name;
    node.material_system = material_system;

    posteffects_.push_back(node);

    return material_system;
}

PosteffectMaterialSystemBase* PosteffectSystem::create(Context& context, const PosteffectNodeDesc& desc)
{
    PosteffectNode* prev_node = nullptr;

    if (!desc.prev_node.empty())
    {
        for (size_t i = 0, size = posteffects_.size(); i < size; ++i)
        {
            if (posteffects_[i].name == desc.prev_node)
            {
                prev_node = &posteffects_[i];
                break;
            }
        }
        ASSERT(prev_node != nullptr, "Previous node name is set but the node isn't found:" << desc.prev_node);
    }

    PosteffectMaterialSystemBase* material_system = nullptr;
    if (!desc.instantiate)
        material_system = context.material_systems.get<PosteffectMaterialSystemBase>(desc.material.c_str());
    else
    {
        material_system = mhe::create<PosteffectMaterialSystemBase>(context, desc.material, desc.name);
    }
    ASSERT(material_system != nullptr, "Can't find material system with name:" << desc.material);
    material_system->set_priority(posteffect_material_priority_base + desc.priority);
    if (!desc.inputs.empty())
        init_inputs(material_system, context, desc);
    else if (prev_node != nullptr)
    {
        PosteffectMaterialSystemBase* prev_material_system = prev_node->material_system;
        size_t inputs_number = material_system->inputs_number();
        size_t outputs_number = prev_material_system->outputs_number();
        if (outputs_number == 0)
        {
            material_system->init_screen_input(context, 0);
        }
        size_t n = min(inputs_number, outputs_number);
        for (size_t i = 0; i < n; ++i)
            material_system->set_input(i, prev_material_system->output(i));
    }
    else
    {
        material_system->init_screen_input(context, 0);
    }

    if (!desc.outputs.empty())
        init_outputs(material_system, context, desc);

    init_buffers(material_system, context, desc);
    init_uniforms(material_system, context, desc);

    material_system->postinit(context);
    return material_system;
}

void PosteffectSystem::init_inputs(PosteffectMaterialSystemBase* material_system, Context& context, 
    const PosteffectSystem::PosteffectNodeDesc& node_desc)
{
    for (size_t i = 0, size = node_desc.inputs.size(); i < size; ++i)
    {
        const NodeInput& input = node_desc.inputs[i];
        if (is_handle_valid(input.explicit_texture.id))
            material_system->set_input(input.index, input.explicit_texture);
        else if (!input.node.empty())
        {
            const PosteffectNode* node = find_node(input.node);
            ASSERT(node != nullptr && node->material_system != nullptr, "Can't find input node or node is invalid:" << input.node);
            material_system->set_input(input.index, node->material_system->output(input.node_output));
        }
        else if (!input.material.empty())
        {
            MaterialSystem* src_material_system = context.material_systems.get(input.material.c_str());
            ASSERT(src_material_system != nullptr, "Can't find material system:" << input.material);
            TextureInstance texture;
            src_material_system->output(context, input.material_output, texture);
            material_system->set_input(input.index, texture);
        }
        else
        {
            DEBUG_LOG("Material and node for posteffect node " << node_desc.name << " are empty - will copy the framebuffer");
            material_system->init_screen_input(context, input.index,
                input.copy_current_framebuffer ? render_stage_before_submit : render_stage_before_render_target_setup);
        }
    }
}

void PosteffectSystem::init_outputs(PosteffectMaterialSystemBase* material_system, Context& context, 
    const PosteffectSystem::PosteffectNodeDesc& node_desc)
{
    for (size_t i = 0, size = node_desc.outputs.size(); i < size; ++i)
    {
        const NodeOutput& output = node_desc.outputs[i];
        material_system->create_output(context, output.index, output.scale, output.format);
    }
}

void PosteffectSystem::init_buffers(PosteffectMaterialSystemBase* material_system, Context& /*context*/,
    const PosteffectSystem::PosteffectNodeDesc& node_desc)
{
    for (size_t i = 0, size = node_desc.buffers.size(); i < size; ++i)
    {
        const Buffers::type& buffer_desc = node_desc.buffers[i];
        ShaderStorageBufferHandleType buffer_id = buffer_desc.explicit_handle;
        if (!is_handle_valid(buffer_desc.explicit_handle))
        {
            const PosteffectNode* input_node = find_node(buffer_desc.node);
            ASSERT(input_node != nullptr && input_node->material_system != nullptr, "Can't find a node with name:" << buffer_desc.node);
            buffer_id = input_node->material_system->buffer(buffer_desc.node_buffer);
            if (!is_handle_valid(buffer_id))
            {
                WARN_LOG("The buffer is not valid:" << buffer_desc.node << " " << buffer_desc.node_buffer);
            }
        }
        material_system->set_buffer(buffer_desc.index, buffer_id);
    }
}

void PosteffectSystem::init_uniforms(PosteffectMaterialSystemBase* material_system, Context& context,
    const PosteffectSystem::PosteffectNodeDesc& node_desc)
{
    for (size_t i = 0, size = node_desc.uniforms.size(); i < size; ++i)
    {
        const Uniforms::type& buffer_desc = node_desc.uniforms[i];
        UniformBufferHandleType buffer_id = buffer_desc.explicit_handle;
        if (!is_handle_valid(buffer_id))
        {
            if (!buffer_desc.systemwide_name.empty())
            {
                buffer_id = context.renderer->system_wide_uniform(buffer_desc.systemwide_name);
                ASSERT(is_handle_valid(buffer_id), "Invalid system-wide uniform name:" << buffer_desc.systemwide_name);
            }
            else if (!buffer_desc.node.empty())
            {
                const PosteffectNode* input_node = find_node(buffer_desc.node);
                ASSERT(input_node != nullptr && input_node->material_system != nullptr, "Can't find a node with name:" << buffer_desc.node);
                buffer_id = input_node->material_system->uniform(buffer_desc.node_buffer);
                if (!is_handle_valid(buffer_id))
                {
                    WARN_LOG("The uniform is not valid:" << buffer_desc.node << " " << buffer_desc.node_buffer);
                }
            }
            else if (!buffer_desc.material.empty())
            {
                MaterialSystem* material_system = context.material_systems.get(buffer_desc.material);
                ASSERT(material_system != nullptr, "Invalid material with name:" << buffer_desc.material);
                buffer_id = material_system->uniform(buffer_desc.node_buffer);
                if (!is_handle_valid(buffer_id))
                {
                    WARN_LOG("The uniform is not valid:" << buffer_desc.material << " " << buffer_desc.node_buffer);
                }
            }
        }
        material_system->set_uniform(buffer_desc.index, buffer_id);
    }
}

void PosteffectSystem::process(Context& context, RenderContext& render_context, SceneContext& scene_context)
{
    for (size_t i = 0, size = posteffects_.size(); i < size; ++i)
        posteffects_[i].material_system->setup_draw_calls(context, scene_context, render_context);
}

const PosteffectSystem::PosteffectNode* PosteffectSystem::find_node(const string& name) const
{
    if (name.empty()) return nullptr;
    for (size_t i = 0, size = posteffects_.size(); i < size; ++i)
    {
        if (posteffects_[i].name == name)
        {
            return &posteffects_[i];
        }
    }
    return nullptr;
}

GISystem::GISystem() :
    rsm_material_system_(nullptr),
    lpv_material_system_(nullptr),
    lpv_resolve_material_system_(nullptr)
{}

void GISystem::add_lpv(Context& context, Renderer& renderer, const LPVParams& params)
{
    const string rsm_name(RSMMaterialSystem::material_name());

    rsm_material_system_ = static_cast<RSMMaterialSystem*>(create(context, rsm_name, rsm_name));
    ASSERT(rsm_material_system_ != nullptr, "RSMMaterialSystem initialization failed."
        "Probably, you forgot to add its description to the configuration file");
    rsm_material_system_->set_priority(params.base_priority - 2);

    const string lpv_name(LPVMaterialSystem::material_name());

    lpv_material_system_ = static_cast<LPVMaterialSystem*>(create(context, lpv_name, lpv_name));
    lpv_material_system_->set_gbuffer(rsm_material_system_->gbuffer());
    lpv_material_system_->settings().mode = LPVMaterialSystem::mode_rsm;
    lpv_material_system_->set_priority(params.base_priority - 1);
    ASSERT(lpv_material_system_ != nullptr, "LPVMaterialSystem initialization failed."
        "Probably, you forgot to add its description to the configuration file");

    const string lpv_resolve_name(LPVResolveMaterialSystem::material_name());
    PosteffectSystem::PosteffectNodeDesc posteffect_node_desc;
    posteffect_node_desc.name = lpv_resolve_name;
    posteffect_node_desc.material = lpv_resolve_name;
    posteffect_node_desc.priority = 0;
    // input 0 - normals
    PosteffectSystem::NodeInput& input0 = posteffect_node_desc.inputs.add();
    input0.index = 0;
    input0.explicit_texture = renderer.scene_normals_buffer();
    // input 1 - depth
    PosteffectSystem::NodeInput& input1 = posteffect_node_desc.inputs.add();
    input1.index = 1;
    input1.explicit_texture = renderer.scene_depth_buffer();
    // ...and 3 channels of LPV RT
    PosteffectSystem::NodeInput& input2 = posteffect_node_desc.inputs.add();
    input2.index = 2;
    input2.material = lpv_name;
    input2.material_output = 0;
    PosteffectSystem::NodeInput& input3 = posteffect_node_desc.inputs.add();
    input3.index = 3;
    input3.material = lpv_name;
    input3.material_output = 1;
    PosteffectSystem::NodeInput& input4 = posteffect_node_desc.inputs.add();
    input4.index = 4;
    input4.material = lpv_name;
    input4.material_output = 2;
    // resolve-to buffer
    PosteffectSystem::NodeOutput& output0 = posteffect_node_desc.outputs.add();
    output0.format = params.output_texture_format;
    output0.index = 0;
    output0.scale = params.output_texture_scale;
    // we also need a couple of uniforms
    PosteffectSystem::Uniforms::type& uniform0 = posteffect_node_desc.uniforms.add();
    uniform0.index = 0;
    uniform0.explicit_handle = renderer.render_context().main_camera.percamera_uniform;
    PosteffectSystem::Uniforms::type& uniform1 = posteffect_node_desc.uniforms.add();
    uniform1.index = 1;
    uniform1.explicit_handle = lpv_material_system_->injection_settings_uniform();

    lpv_resolve_material_system_ = static_cast<LPVResolveMaterialSystem*>(
        renderer.posteffect_system().create(context, posteffect_node_desc));
    ASSERT(lpv_resolve_material_system_ != nullptr, "LPVResolveMaterialSystem initialization failed."
        "Probably, you forgot to add its description to the configuration file");
}

void GISystem::apply(Renderer& renderer)
{
    renderer.set_gi_modifier_material_system(lpv_resolve_material_system_, 0);
}

void GISystem::before_render(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    if (rsm_material_system_ != nullptr)
        rsm_material_system_->start_frame(context, scene_context, render_context);
}

void GISystem::render(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    if (rsm_material_system_ != nullptr && lpv_material_system_ != nullptr)
    {
        RSMData data;
        rsm_material_system_->rsm_data(data);
        lpv_material_system_->set_rsm_data(data);

        rsm_material_system_->setup_draw_calls(context, scene_context, render_context);
        lpv_material_system_->setup_draw_calls(context, scene_context, render_context);
    }
}

Renderer::Renderer(Context& context) :
    context_(context),
    skybox_material_system_(nullptr),
    shadowmap_depth_write_material_system_(nullptr),
    directional_shadowmap_depth_write_material_system_(nullptr),    
    transparent_objects_material_system_(nullptr),
    particles_material_system_(nullptr),
    fullscreen_debug_material_system_(nullptr),
    ambient_color_(0.1f, 0.1f, 0.1f, 0.1f),
    debug_mode_(renderer_debug_mode_none)
{}

bool Renderer::init()
{
    UniformBuffer& buffer = create_and_get(context_.uniform_pool);
    render_context_.main_camera.percamera_uniform = buffer.id();
    UniformBufferDesc desc;
    desc.unit = perframe_data_unit;
    desc.size = sizeof(PerCameraData);
    buffer.init(desc);

    context_.renderer = this;
    return true;
}

void Renderer::destroy()
{
    destroy_pool_object(context_.uniform_pool, render_context_.main_camera.percamera_uniform);
}

void Renderer::update(SceneContext& scene_context)
{
    GPU_PROFILE("update");
    PerCameraData data;
    data.vp = render_context_.main_camera.vp;
    data.view = render_context_.main_camera.view;
    data.proj = render_context_.main_camera.proj;
    data.inv_vp = render_context_.main_camera.inv_vp;
    data.inv_proj = render_context_.main_camera.proj.inverted();
    data.viewpos = vec4(render_context_.main_camera.viewpos, 0.0f);

    data.ambient = ambient_color_;
    data.znear = render_context_.main_camera.znear;
    data.zfar = render_context_.main_camera.zfar;
    data.inv_viewport = vec2(1.0f / context_.window_system.width(), 1.0f / context_.window_system.height());
    data.viewport = context_.window_system.screen_size();

    UniformBuffer& buffer = context_.uniform_pool.get(render_context_.main_camera.percamera_uniform);
    buffer.update(data);

    update_nodes(context_, render_context_, scene_context);
    update_lights(context_, render_context_, scene_context);
    update_impl(context_, render_context_, scene_context);
}

void Renderer::before_update(SceneContext& scene_context)
{
    MainGPUProfiler::instance().clear();

    render_context_.render_view_requests.reset();
    if (directional_shadowmap_depth_write_material_system_ != nullptr)
        directional_shadowmap_depth_write_material_system_->start_frame(context_, scene_context, render_context_);

    for (size_t i = 0, size = material_systems_.size(); i < size; ++i)
        material_systems_[i]->start_frame(context_, scene_context, render_context_);

    gi_system_.before_render(context_, scene_context, render_context_);
}

void Renderer::render(SceneContext& scene_context)
{
    if (skybox_material_system_ != nullptr)
        skybox_material_system_->setup_draw_calls(context_, scene_context, render_context_);
    if (directional_shadowmap_depth_write_material_system_ != nullptr)
        directional_shadowmap_depth_write_material_system_->setup_draw_calls(context_, scene_context, render_context_);
    if (shadowmap_depth_write_material_system_ != nullptr)
        shadowmap_depth_write_material_system_->setup_draw_calls(context_, scene_context, render_context_);
    render_impl(context_, render_context_, scene_context);

    for (size_t i = 0, size = material_systems_.size(); i < size; ++i)
        material_systems_[i]->setup_draw_calls(context_, scene_context, render_context_);

    gi_system_.render(context_, scene_context, render_context_);

    posteffect_system_.process(context_, render_context_, scene_context);

    if (fullscreen_debug_material_system_ != nullptr)
        fullscreen_debug_material_system_->setup_draw_calls(context_, scene_context, render_context_);

    sort_draw_calls(context_, render_context_);
    execute_render(render_context_);
}

void Renderer::execute_render(RenderContext& render_context)
{
    ProfilerElement pe("engine.render");
    context_.driver.begin_render();
    context_.driver.clear_color();
    context_.driver.clear_depth();

    {
        GPU_PROFILE("main_pass");
        context_.driver.render(context_, render_context.draw_calls.data(), render_context.draw_calls.size());
    }
    {
        context_.driver.render(context_, render_context.explicit_draw_calls.data(), render_context.explicit_draw_calls.size());
    }
}

void Renderer::flush()
{
    {
        ProfilerElement end_render_pe("driver.end_render");
        GPU_PROFILE("flush");
        context_.driver.end_render();
    }
    {
        ProfilerElement swap_buffers_pe("window_system.swap_buffers");
        context_.window_system.swap_buffers();
    }

    render_context_.draw_calls.clear();
    render_context_.explicit_draw_calls.clear();

    context_.render_target_manager.free_all_temp_render_targets();

    MainGPUProfiler::instance().update();
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

void Renderer::set_directional_shadowmap_depth_write_material_system(MaterialSystem* material_system)
{
    directional_shadowmap_depth_write_material_system_ = material_system;
    directional_shadowmap_depth_write_material_system_->set_priority(shadowmap_depth_write_material_system_priority);
}

void Renderer::set_fullscreen_debug_material_system(PosteffectDebugMaterialSystem* material_system)
{
    fullscreen_debug_material_system_ = material_system;
    fullscreen_debug_material_system_->set_priority(debug_material_system_priority);
    fullscreen_debug_material_system_->disable();
}

void Renderer::debug_mode_changed(DebugMode mode, MaterialSystemId material_system_id)
{
    if (fullscreen_debug_material_system_ == nullptr) return;

    if (mode == renderer_debug_mode_shadows)
    {
        RenderTarget& render_target = context_.render_target_pool.get(directional_shadowmap_depth_write_material_system_->render_target_id());
        fullscreen_debug_material_system_->set_render_target(render_target);
        fullscreen_debug_material_system_->enable();
    }

    if (mode == renderer_debug_mode_posteffect)
    {
        PosteffectMaterialSystemBase* posteffect_material_system = context_.material_systems.get<PosteffectMaterialSystemBase>(material_system_id);
        for (size_t i = 0, n = posteffect_material_system->outputs_number(); i < n; ++i)
            fullscreen_debug_material_system_->set_texture(i, posteffect_material_system->output(i));
        fullscreen_debug_material_system_->set_viewports_number(posteffect_material_system->outputs_number());
        fullscreen_debug_material_system_->enable();
    }
}

UniformBufferHandleType Renderer::system_wide_uniform(const string& name) const
{
    if (name == "main_camera")
        return render_context_.main_camera.percamera_uniform;
    return InvalidHandle<UniformBufferHandleType>::id;
}

bool load_node(NodeInstance& node, const string& name, const char* material_system_name, Context& context, SceneContext& scene_context)
{
    if (!context.mesh_manager.get_instance(node.mesh, name))
    {
        ERROR_LOG("load_node() failed: can not get mesh:" << name);
        return false;
    }

    AABBInstance& aabb_instance = scene_context.aabb_pool.get(node.aabb_id);
    aabb_instance.aabb = node.mesh.mesh.aabb;

    MaterialSystem* material_system = context.material_systems.get(material_system_name);
    if (material_system == nullptr)
        return false;

    std::vector<ModelContext> model_contexts(node.mesh.instance_parts.size());
    for (size_t i = 0; i < node.mesh.instance_parts.size(); ++i)
    {
        ModelContext& model_context = model_contexts[i];
        model_context.transform_uniform = node.mesh.shared_uniform;
        model_context.animation_texture_buffer = node.mesh.skeleton_instance.texture_buffer;
        model_context.baked_light_texture_buffer = node.mesh.gi_data.texture_buffer;
        AABBInstance& part_aabb_instance = create_and_get(scene_context.parts_aabb_pool);
        part_aabb_instance.aabb = node.mesh.mesh.parts[i].aabb;
        node.mesh.instance_parts[i].aabb_id = part_aabb_instance.id;
        part_aabb_instance.parent_id = aabb_instance.id;
    }
    material_system->setup(context, scene_context, &node.mesh.instance_parts[0], &node.mesh.mesh.parts[0], &model_contexts[0], node.mesh.instance_parts.size());
    return true;
}

bool init_render(Context& context)
{
    RenderGlobals render_globals;
    setup(render_globals);
    init_standart_layouts(context);

    return true;
}

template <class Pool>
void destroy_pool_elements(Pool& pool)
{
    typename Pool::type* p = pool.all_objects();
    size_t size = pool.size();
    for (size_t i = 0; i < size; ++i)
        p[i].close();
}

// TODO: looks like crap. I'll have to rename all methods from close() to destroy()
template <class Pool>
void destroy_pool_elements_destroy(Pool& pool)
{
    typename Pool::type* p = pool.all_objects();
    size_t size = pool.size();
    for (size_t i = 0; i < size; ++i)
        p[i].destroy();
}

void destroy_render(Context& context)
{
    context.material_systems.clear(context);
    // tear down the context
    destroy_pool_elements(context.vertex_buffer_pool);
    destroy_pool_elements(context.index_buffer_pool);
    destroy_pool_elements(context.uniform_pool);
    destroy_pool_elements(context.shader_pool);
    destroy_pool_elements(context.texture_pool);
    destroy_pool_elements(context.render_target_pool);
    destroy_pool_elements(context.shader_storage_buffer_pool);
    destroy_pool_elements_destroy(context.texture_buffer_pool);
}

}
