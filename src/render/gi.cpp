#include "render/gi.hpp"

#include "render/context.hpp"
#include "render/draw_call.hpp"
#include "render/layouts.hpp"
#include "render/utils/simple_meshes.hpp"
#include "render/deferred_renderer.hpp"
#include "render/rsm_material_system.hpp"
#include "render/lpv_material_system.hpp"
#include "render/skybox_material_system.hpp"
#include "math/sh.h"
#include "debug/debug_views.hpp"

namespace mhe {

bool CubemapIntegrator::init(Context& context, const Settings& settings)
{
    Shader shader;
    if (!context.shader_manager.get(shader, settings.shader_name))
    {
        ERROR_LOG("Can't load the shader:" << settings.shader_name);
        return false;
    }
    ubershader_id_ = shader.shader_program_handle;

    UniformBuffer& uniform_buffer = create_and_get(context.uniform_pool);
    UniformBufferDesc desc;
    desc.size = sizeof(vec4);
    desc.update_type = uniform_buffer_normal;
    desc.unit = 2;
    if (!uniform_buffer.init(desc))
    {
        ERROR_LOG("Can't initialize a uniform buffer");
        return false;
    }
    settings_uniform_id_ = uniform_buffer.id();

    return true;
}

bool CubemapIntegrator::integrate(ShaderStorageBuffer& dst, Context& context, Texture& cubemap)
{
    UberShader& ubershader = context.ubershader_pool.get(ubershader_id_);

    ComputeCallExplicit compute_call;
    prepare_draw_call(compute_call);
    compute_call.image_access[0] = access_readonly;
    compute_call.images[0] = &cubemap;
    compute_call.shader_program = &context.shader_pool.get(ubershader.get_default());
    size_t threads_number = compute_call.shader_program->variable_value<size_t>(string("THREADS"));
    size_t threads_number_sq = threads_number * threads_number;
    // each thread processes THREADS number of pixels in each dimension
    compute_call.workgroups_number.set(iceil(cubemap.width(), threads_number_sq), iceil(cubemap.height(), threads_number_sq), 1);
    compute_call.barrier = memory_barrier_storage_buffer;

    ShaderStorageBuffer& tmp_buffer = create_and_get(context.shader_storage_buffer_pool);
    ShaderStorageBufferDesc desc;
    desc.format = format_float;
    desc.size = compute_call.workgroups_number.x() * compute_call.workgroups_number.y() * sizeof(SH<vec3, 3>);
    desc.update_type = buffer_update_type_dynamic;

    if (!tmp_buffer.init(desc, nullptr, 0))
    {
        ERROR_LOG("ShaderStorageBuffer initialization failed");
        return false;
    }

    compute_call.buffers[0] = &tmp_buffer;

    context.driver.execute(context, &compute_call, 1);

    // reduction step
    const UberShader::Info& info = ubershader.info("STAGE");
    UberShader::Index index;
    index.set(info, 1);
    ShaderProgram& reduction_shader_program = context.shader_pool.get(ubershader.get(index));
    size_t reduction_threads_number = reduction_shader_program.variable_value<size_t>("REDUCTION_THREADS");

    UniformBuffer& uniform_buffer = context.uniform_pool.get(settings_uniform_id_);
    size_t iterations = iceil(compute_call.workgroups_number.x() * compute_call.workgroups_number.y(), reduction_threads_number);
    vec4 data(static_cast<float>(iterations), sh9_calculate_cubemap_projection_weight(cubemap.width(), cubemap.height()), 0.0f, 0.0f);
    uniform_buffer.update(data);

    compute_call.buffers[0] = &tmp_buffer;
    compute_call.buffers[1] = &dst;
    compute_call.uniforms[2] = &uniform_buffer;
    compute_call.shader_program = &reduction_shader_program;
    compute_call.workgroups_number.set(1);

    context.driver.execute(context, &compute_call, 1);

    tmp_buffer.close();
    context.shader_storage_buffer_pool.remove(tmp_buffer.id());

    return true;
}

bool IndirectLightingResolveMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    set_layout(FullscreenLayout::handle);

    const string& diffuse_resolve_shader = material_system_context.options.get<string>("diffuse_resolve_shader");
    ASSERT(!diffuse_resolve_shader.empty(), "Diffuse resolve shader name must not be empty");
    Shader shader;
    if (!context.shader_manager.get(shader, diffuse_resolve_shader))
    {
        ERROR_LOG("Can't load shader:" << diffuse_resolve_shader);
        return false;
    }
    diffuse_resolve_ubershader_id_ = shader.shader_program_handle;

    const string& specular_resolve_shader = material_system_context.options.get<string>("specular_resolve_shader");
    ASSERT(!specular_resolve_shader.empty(), "Specular resolve shader name must not be empty");
    if (!context.shader_manager.get(shader, specular_resolve_shader))
    {
        ERROR_LOG("Can't load shader:" << specular_resolve_shader);
        return false;
    }
    specular_resolve_ubershader_id_ = shader.shader_program_handle;

    RenderTargetDesc rt_desc;
    rt_desc.color_datatype[0] = format_default;
    rt_desc.color_format[0] = format_rgb16f;
    rt_desc.color_targets = 1;
    rt_desc.target = rt_readwrite;
    rt_desc.texture_type = texture_2d;
    rt_desc.use_depth = false;
    rt_desc.use_stencil = false;
    resolved_diffuse_rt_id_ = context.render_target_manager.create(context, rt_desc, 1.0f).id();
    resolved_specular_rt_id_ = context.render_target_manager.create(context, rt_desc, 1.0f).id();

    if (!init_fullscreen_quad(diffuse_resolve_quad_mesh_, context))
    {
        ERROR_LOG("Fullscreen quad initialization failed");
        return false;
    }

    diffuse_resolve_material_id_ = context.materials[id()].create();
    diffuse_resolve_quad_mesh_.instance_parts[0].material.id = diffuse_resolve_material_id_;

    specular_resolve_quad_mesh_ = diffuse_resolve_quad_mesh_;
    specular_resolve_material_id_ = context.materials[id()].create();
    specular_resolve_quad_mesh_.instance_parts[0].material.id = specular_resolve_material_id_;

    global_ambient_sh_id_ = ShaderStorageBuffer::invalid_id;

    return true;
}

bool IndirectLightingResolveMaterialSystem::init_fullscreen_quad(MeshInstance& mesh_instance, Context& context)
{
    if (!utils::create_fullscreen_quad_shared(mesh_instance, context))
    {
        ERROR_LOG("Can't create a fullscreen quad");
        return false;
    }

    RenderState& render_state = create_and_get(context.render_state_pool);
    RenderStateDesc render_state_desc;
    render_state_desc.depth.test_enabled = false;
    render_state_desc.depth.write_enabled = false;
    render_state_desc.rasterizer.cull = cull_none;
    if (!render_state.init(render_state_desc))
    {
        ERROR_LOG("Can't initialize a render state for ProbesAccumulator");
        return false;
    }

    mesh_instance.mesh.parts[0].render_data.layout = FullscreenLayout::handle;
    mesh_instance.instance_parts[0].render_state_id = render_state.id();

    mesh_instance.instance_parts[0].material.material_system = id();
    return true;
}

void IndirectLightingResolveMaterialSystem::destroy(Context& context)
{
    destroy_pool_object(context.render_state_pool, diffuse_resolve_quad_mesh_.instance_parts[0].render_state_id);
    context.render_target_manager.destroy(resolved_diffuse_rt_id_, context);
    context.render_target_manager.destroy(resolved_specular_rt_id_, context);
}

void IndirectLightingResolveMaterialSystem::init_debug_views(Context& context)
{
    RenderTarget& diffuse_rt = context.render_target_pool.get(resolved_diffuse_rt_id_);
    TextureInstance texture;
    diffuse_rt.color_texture(texture, 0);
    context.debug_views->add_debug_buffer(string("indirect diffuse"), texture, Renderer::renderer_debug_mode_rgb);

    RenderTarget& specular_rt = context.render_target_pool.get(resolved_specular_rt_id_);
    specular_rt.color_texture(texture, 0);
    context.debug_views->add_debug_buffer(string("indirect specular"), texture, Renderer::renderer_debug_mode_rgb);
}

void IndirectLightingResolveMaterialSystem::setup(Context &context, SceneContext &scene_context, MeshPartInstance* instance_parts,
    MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void IndirectLightingResolveMaterialSystem::update(Context& context, SceneContext& /*scene_context*/, RenderContext& render_context)
{
    // diffuse
    UberShader& ushader = context.ubershader_pool.get(diffuse_resolve_ubershader_id_);
    UberShader::Index index;
    const UberShader::Info& mode_info = ushader.info("MODE");
    const UberShader::Info& gi_texture_info = ushader.info("GI_TEXTURE");
    if (is_handle_valid(global_ambient_sh_id_))
        index.set(mode_info, mode_sh_ambient);
    if (is_handle_valid(gi_diffuse_.id))
        index.set(gi_texture_info, 1);

    Material& material = context.materials[id()].get(diffuse_resolve_material_id_);
    material.shader_program = ushader.get(index);
    material.shader_storage_buffers[2] = global_ambient_sh_id_;
    context.renderer->setup_common_pass(material);

    material.textures[4] = gi_diffuse_;

    setup_draw_call(render_context.draw_calls.add(),
        diffuse_resolve_quad_mesh_.instance_parts[0], diffuse_resolve_quad_mesh_.mesh.parts[0], resolved_diffuse_rt_id_);

    // specular
    UberShader& specular_ushader = context.ubershader_pool.get(specular_resolve_ubershader_id_);
    Material& specular_material = context.materials[id()].get(specular_resolve_material_id_);
    specular_material.shader_program = specular_ushader.get_default();
    context.renderer->setup_common_pass(specular_material);
    specular_material.textures[4] = render_context.space_grid.global_cubemap();
    setup_draw_call(render_context.draw_calls.add(),
        specular_resolve_quad_mesh_.instance_parts[0], specular_resolve_quad_mesh_.mesh.parts[0], resolved_specular_rt_id_);
}

GISystem::GISystem() :
rsm_material_system_(nullptr),
    lpv_material_system_(nullptr),
    lpv_resolve_material_system_(nullptr),
    skybox_material_system_(nullptr),
    indirect_lighting_resolve_material_system_(nullptr)
{}

bool GISystem::init(Context& context, const Settings& settings)
{
    const string diffuse_resolve_name = IndirectLightingResolveMaterialSystem::material_name();

    MaterialSystemContext material_system_context;
    material_system_context.material_instances_number = 2;
    material_system_context.priority = DeferredRenderer::deferred_renderer_gbuffer_modifier_priority;
    material_system_context.instance_name = diffuse_resolve_name;
    material_system_context.options.add("diffuse_resolve_shader", settings.diffuse_resolve_shader_name);
    material_system_context.options.add("specular_resolve_shader", settings.specular_resolve_shader_name);

    context.initialization_parameters.add(diffuse_resolve_name, material_system_context);
    indirect_lighting_resolve_material_system_ =
        create<IndirectLightingResolveMaterialSystem>(context, diffuse_resolve_name, diffuse_resolve_name);

    {
        const RenderTarget& rt = context.render_target_pool.get(indirect_lighting_resolve_material_system_->resolved_diffuse_render_target_id());
        TextureInstance texture;
        rt.color_texture(texture, 0);
        context.renderer->set_indirect_diffuse_lighting_texture(texture);
    }

    {
        const RenderTarget& rt = context.render_target_pool.get(indirect_lighting_resolve_material_system_->resolved_specular_render_target_id());
        TextureInstance texture;
        rt.color_texture(texture, 0);
        context.renderer->set_indirect_specular_lighting_texture(texture);
    }

    return true;
}

void GISystem::destroy(Context& context)
{
    UNUSED(context);
}

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
    lpv_resolve_material_system_->set_priority(params.base_priority);
    indirect_lighting_resolve_material_system_->add_gi_diffuse(lpv_resolve_material_system_->output(0));
}

void GISystem::apply(Renderer& renderer)
{
    ASSERT(indirect_lighting_resolve_material_system_ != nullptr, "Invalid diffuse GI material system");
    indirect_lighting_resolve_material_system_->set_global_ambient_sh_buffer(ambient_sh_buffer_id_);
}

void GISystem::add_skybox(Context& context, const SkyboxMaterialSystem* skybox_material_system, const CubemapIntegrator::Settings& integrator_settings)
{
    skybox_material_system_ = skybox_material_system;
    cubemap_integrator_.init(context, integrator_settings);
    if (skybox_material_system_ != nullptr)
    {
        ShaderStorageBuffer& buffer = create_and_get(context.shader_storage_buffer_pool);
        ShaderStorageBufferDesc desc;
        desc.format = format_float;
        desc.size = sizeof(SH<vec3, 3>);
        desc.update_type = buffer_update_type_dynamic;
        if (!buffer.init(desc, nullptr, 0))
        {
            ERROR_LOG("Can't initialize a ShaderStorageBuffer");
        }
        ambient_sh_buffer_id_ = buffer.id();

        cubemap_integrator_.integrate(context.shader_storage_buffer_pool.get(ambient_sh_buffer_id_),
            context, context.texture_pool.get(skybox_material_system_->skybox_texture().id));
    }
}

void GISystem::update_skybox(Context& context)
{
    if (skybox_material_system_ != nullptr)
        cubemap_integrator_.integrate(context.shader_storage_buffer_pool.get(ambient_sh_buffer_id_),
        context, context.texture_pool.get(skybox_material_system_->skybox_texture().id));
}

void GISystem::before_render(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    if (rsm_material_system_ != nullptr)
        rsm_material_system_->start_frame(context, scene_context, render_context);
}

void GISystem::render(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    if (rsm_material_system_ != nullptr && lpv_material_system_ != nullptr && lpv_resolve_material_system_ != nullptr)
    {
        RSMData data;
        rsm_material_system_->rsm_data(data);
        lpv_material_system_->set_rsm_data(data);

        rsm_material_system_->setup_draw_calls(context, scene_context, render_context);
        lpv_material_system_->setup_draw_calls(context, scene_context, render_context);

        lpv_resolve_material_system_->setup_draw_calls(context, scene_context, render_context);
    }

    indirect_lighting_resolve_material_system_->setup_draw_calls(context, scene_context, render_context);
}

}
