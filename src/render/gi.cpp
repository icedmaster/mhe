#include "render/gi.hpp"

#include "render/context.hpp"
#include "render/draw_call.hpp"
#include "render/layouts.hpp"
#include "render/utils/simple_meshes.hpp"
#include "render/renderer.hpp"
#include "math/sh.h"

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
    compute_call.workgroups_number.set(cubemap.width() / threads_number_sq, cubemap.height() / threads_number_sq, 1);
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
    size_t iterations = compute_call.workgroups_number.x() * compute_call.workgroups_number.y() / reduction_threads_number;
    vec4 data(static_cast<float>(iterations), sh9_calculate_cubemap_projection_weight(cubemap.width(), cubemap.height()), 0.0f, 0.0f);
    uniform_buffer.update(data);

    compute_call.buffers[0] = &tmp_buffer;
    compute_call.buffers[1] = &dst;
    compute_call.uniforms[2] = &uniform_buffer;
    compute_call.shader_program = &reduction_shader_program;
    compute_call.workgroups_number.set(1);

    context.driver.execute(context, &compute_call, 1);

    context.shader_storage_buffer_pool.remove(tmp_buffer.id());

    return true;
}

bool IndirectLightingResolveMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    set_layout(FullscreenLayout::handle);
    if (!init_default(context, material_system_context))
        return false;

    RenderTargetDesc rt_desc;
    rt_desc.color_datatype[0] = format_default;
    rt_desc.color_format[0] = format_rgb16f;
    rt_desc.color_targets = 1;
    rt_desc.target = rt_readwrite;
    rt_desc.texture_type = texture_2d;
    rt_desc.use_depth = false;
    rt_desc.use_stencil = false;
    rt_id_ = context.render_target_manager.create(context, rt_desc, 1.0f).id();

    if (!init_fullscreen_quad(context))
    {
        ERROR_LOG("Fullscreen quad initialization failed");
        return false;
    }

    global_ambient_sh_id_ = ShaderStorageBuffer::invalid_id;

    return true;
}

bool IndirectLightingResolveMaterialSystem::init_fullscreen_quad(Context& context)
{
    if (!utils::create_fullscreen_quad_shared(quad_mesh_, context))
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

    quad_mesh_.mesh.parts[0].render_data.layout = FullscreenLayout::handle;
    quad_mesh_.instance_parts[0].render_state_id = render_state.id();

    quad_mesh_.instance_parts[0].material.material_system = id();
    quad_mesh_.instance_parts[0].material.id = context.materials[id()].create();
    return true;
}

void IndirectLightingResolveMaterialSystem::destroy(Context& context)
{
    destroy_pool_object(context.render_state_pool, quad_mesh_.instance_parts[0].render_state_id);
    context.render_target_manager.destroy(rt_id_, context);
}

void IndirectLightingResolveMaterialSystem::setup(Context &context, SceneContext &scene_context, MeshPartInstance* instance_parts,
    MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void IndirectLightingResolveMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    UberShader& ushader = ubershader(context);
    UberShader::Index index;
    const UberShader::Info& info = ushader.info("MODE");
    if (is_handle_valid(global_ambient_sh_id_))
        index.set(info, mode_sh_ambient);

    Material& material = context.materials[id()].get(quad_mesh_.instance_parts[0].material.id);
    material.shader_program = ushader.get(index);
    material.shader_storage_buffers[2] = global_ambient_sh_id_;
    context.renderer->setup_common_pass(material);

    setup_draw_call(render_context.draw_calls.add(), quad_mesh_.instance_parts[0], quad_mesh_.mesh.parts[0], rt_id_);
}

}