#include "render/gi.hpp"

#include "render/context.hpp"
#include "render/draw_call.hpp"
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

}