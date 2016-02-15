#include "render/lpv_material_system.hpp"

#include "render/context.hpp"
#include "render/render_context.hpp"
#include "utils/global_log.hpp"

namespace mhe {

bool LPVMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    const string& injection_shader_name = material_system_context.options.get<string>("injection_shader");
    ASSERT(!injection_shader_name.empty(), "Injection shader name must not be empty");
    Shader shader;
    bool res = context.shader_manager.get(shader, injection_shader_name);
    ASSERT(res, "Can't get the injection shader for LPV");
    injection_shader_ = context.ubershader_pool.get(shader.shader_program_handle).get_default();

    pool_initializer<RenderTargetPool> rt_wr(context.render_target_pool);
    RenderTargetDesc rt_desc;
    rt_desc.width = rt_desc.height = rt_desc.depth = settings_.size;
    rt_desc.color_targets = 3;
    rt_desc.target = rt_readwrite;
    rt_desc.use_depth = false;
    rt_desc.use_stencil = false;
    rt_desc.texture_type = texture_3d;
    for (size_t i = 0; i < 3; ++i)
    {
        rt_desc.color_format[i] = format_rgba16f;
        rt_desc.color_datatype[i] = format_default;
    }
    if (!rt_wr.object().init(context, rt_desc))
    {
        ERROR_LOG("Can't initialize a RT for LPV");
        return false;
    }

    pool_initializer<RenderStatePool> injection_render_state_wr(context.render_state_pool);
    RenderStateDesc render_state_desc;
    render_state_desc.depth.test_enabled = false;
    render_state_desc.depth.write_enabled = false;
    render_state_desc.rasterizer.cull = cull_none;
    render_state_desc.viewport.viewport.set(0, 0, settings_.size, settings_.size);

    render_state_desc.blend.enabled = true;
    render_state_desc.blend.func = blend_max;
    render_state_desc.blend.func_alpha = blend_max;
    render_state_desc.blend.srcmode = blend_one;
    render_state_desc.blend.srcmode_alpha = blend_one;
    render_state_desc.blend.dstmode = blend_one;
    render_state_desc.blend.dstmode_alpha = blend_one;
    if (!injection_render_state_wr.object().init(render_state_desc))
    {
        ERROR_LOG("Can't initialize a render state for LPV");
        return false;
    }

    pool_initializer<UniformPool> injection_uniform_wr(context.uniform_pool);
    UniformBufferDesc uniform_desc;
    uniform_desc.unit = 0;
    uniform_desc.update_type = uniform_buffer_normal;
    uniform_desc.size = sizeof(InjectionShaderData);
    if (!injection_uniform_wr.object().init(uniform_desc))
    {
        ERROR_LOG("Can't initialize a uniform buffer for injection");
        return false;
    }

    Material& injection_material = create_and_get(context.materials[id()]);
    injection_material.shader_program = injection_shader_;
    injection_material.uniforms[0] = injection_uniform_wr.get();
    injection_material_ = injection_material.id;

    render_target_grid_ = rt_wr.take();
    injection_render_state_ = injection_render_state_wr.take();
    injection_uniform_ = injection_uniform_wr.take();

    return true;
}

void LPVMaterialSystem::destroy(Context& context)
{
    destroy_pool_object(context.uniform_pool, injection_uniform_);
    destroy_pool_object(context.render_state_pool, injection_render_state_);
    destroy_pool_object(context.render_target_pool, render_target_grid_);
}

void LPVMaterialSystem::setup(Context &context, SceneContext &scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void LPVMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    ASSERT(is_handle_valid(gbuffer_.accumulator.id) && is_handle_valid(gbuffer_.normal.id) &&
           is_handle_valid(gbuffer_.depth.id), "Invalid GBuffer");
    // each pixel is a virtual point light
    Texture& texture = context.texture_pool.get(gbuffer_.accumulator.id);
    size_t vpl_number = texture.width() * texture.height();

    injection(render_context.draw_calls.add(), context, render_context, vpl_number);
}

void LPVMaterialSystem::injection(DrawCall& draw_call, Context& context, RenderContext& render_context,
                                  size_t vpl_number)
{
    clear_command_.reset();

    UniformBuffer& uniform = context.uniform_pool.get(injection_uniform_);
    InjectionShaderData shader_data;
    shader_data.settings.set_x(static_cast<float>(settings_.size));
    shader_data.rsm_to_ws = rsm_data_.vp.inverted();
    shader_data.ws_to_lpv = calculate_lpv_transform(render_context);
    uniform.update(shader_data);

    Material& material = context.materials[id()].get(injection_material_);
    material.textures[0] = gbuffer_.normal;
    material.textures[1] = gbuffer_.accumulator;
    material.textures[2] = gbuffer_.depth;

    draw_call.material.id = material.id;
    draw_call.material.material_system = id();

    draw_call.render_data.elements_number = vpl_number;
    draw_call.render_data.primitive = gpu_generated;

    draw_call.render_state = injection_render_state_;

    draw_call.render_target = render_target_grid_;

    draw_call.pass = 0;
    draw_call.command = &clear_command_;
}

mat4x4 LPVMaterialSystem::calculate_lpv_transform(const RenderContext& render_context)
{
    vec3 scene_min, scene_max;
    render_context.aabb.min_max(scene_min, scene_max);
    float diagonal_length = (scene_max - scene_min).length();
    mat4x4 lpv_transform = mat4x4::translation_matrix(-scene_min) * mat4x4::scaling_matrix(1.0f / diagonal_length);
    return lpv_transform;
}

void LPVMaterialSystem::output(Context& context, size_t unit, TextureInstance& texture) const
{
    RenderTarget& rt = context.render_target_pool.get(render_target_grid_);
    rt.color_texture(texture, unit);
}

}
