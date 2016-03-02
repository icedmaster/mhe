#include "render/lpv_material_system.hpp"

#include "render/context.hpp"
#include "render/render_context.hpp"
#include "render/render_globals.hpp"
#include "utils/global_log.hpp"
#include "debug/debug_views.hpp"

namespace mhe {

bool LPVMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    const string& injection_shader_name = material_system_context.options.get<string>("injection_shader");
    ASSERT(!injection_shader_name.empty(), "Injection shader name must not be empty");
    Shader shader;
    bool res = context.shader_manager.get(shader, injection_shader_name);
    ASSERT(res, "Can't get the injection shader for LPV");
    injection_shader_ = context.ubershader_pool.get(shader.shader_program_handle).get_default();

    const string& propagation_shader_name = material_system_context.options.get<string>("propagation_shader");
    ASSERT(!propagation_shader_name.empty(), "Propagation shader name must not be empty");
    res = context.shader_manager.get(shader, propagation_shader_name);
    ASSERT(res, "Can't get the propagation shader for LPV");
    const UberShader& propagation_ubershader = context.ubershader_pool.get(shader.shader_program_handle);
    propagation_shader_ = propagation_ubershader.get_default();
    const UberShader::Info& occlusion_info = propagation_ubershader.info("OCCLUSION");
    UberShader::Index ubershader_index;
    ubershader_index.set(occlusion_info, 1);
    propagation_shader_with_occlusion_ = propagation_ubershader.get(ubershader_index);

    const string& geometry_injection_shader_name = material_system_context.options.get<string>("geometry_injection_shader");
    ASSERT(!geometry_injection_shader_name.empty(), "Geometry injection shader name must not be empty");
    res = context.shader_manager.get(shader, geometry_injection_shader_name);
    ASSERT(res, "Can't get the geometry injection shader for LPV");
    geometry_injection_shader_ = context.ubershader_pool.get(shader.shader_program_handle).get_default();

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
        rt_desc.color_filter[i] = texture_filter_linear;
        rt_desc.color_address_mode[i] = texture_border;
    }
    if (!rt_wr.object().init(context, rt_desc))
    {
        ERROR_LOG("Can't initialize a RT for LPV");
        return false;
    }

    pool_initializer<RenderTargetPool> scnd_rt_wr(context.render_target_pool);
    if (!scnd_rt_wr.object().init(context, rt_desc))
    {
        ERROR_LOG("Can't initialize a RT for LPV propagation");
        return false;
    }

    pool_initializer<RenderTargetPool> occlusion_rt_wr(context.render_target_pool);
    rt_desc.color_targets = 1;
    if (!occlusion_rt_wr.object().init(context, rt_desc))
    {
        ERROR_LOG("Can't initialize an occlusion RT for LPV");
       return false;
    }
    TextureInstance occlusion_texture;
    occlusion_rt_wr.object().color_texture(occlusion_texture, 0);

    pool_initializer<RenderStatePool> injection_render_state_wr(context.render_state_pool);
    RenderStateDesc render_state_desc;
    render_state_desc.depth.test_enabled = false;
    render_state_desc.depth.write_enabled = false;
    render_state_desc.rasterizer.cull = cull_none;
    render_state_desc.viewport.viewport.set(0, 0, settings_.size, settings_.size);

    render_state_desc.blend.enabled = true;
    render_state_desc.blend.func = blend_add;
    render_state_desc.blend.func_alpha = blend_add;
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

    Material& propagation_material = create_and_get(context.materials[id()]);
    propagation_material.shader_program = propagation_shader_with_occlusion_;
    propagation_material.uniforms[0] = injection_uniform_wr.get();
    rt_wr.object().color_textures(propagation_material.textures);
    propagation_material.textures[3] = occlusion_texture;
    propagation_material_[0] = propagation_material.id;

    Material& propagation_material2 = create_and_get(context.materials[id()]);
    propagation_material2.shader_program = propagation_shader_with_occlusion_;
    propagation_material2.uniforms[0] = injection_uniform_wr.get();
    scnd_rt_wr.object().color_textures(propagation_material2.textures);
    propagation_material.textures[3] = occlusion_texture;
    propagation_material_[1] = propagation_material2.id;

    Material& geometry_injection_material = create_and_get(context.materials[id()]);
    geometry_injection_material.shader_program = geometry_injection_shader_;
    geometry_injection_material.uniforms[0] = injection_uniform_wr.get();
    geometry_injection_material_ = geometry_injection_material.id;

    occlusion_render_target_ = occlusion_rt_wr.take();
    render_target_grid_ = rt_wr.take();
    injection_render_state_ = injection_render_state_wr.take();
    injection_uniform_ = injection_uniform_wr.take();
    propagation_render_target_ = scnd_rt_wr.take();

    RenderTargetHandleType render_targets[2] = {propagation_render_target_, render_target_grid_};
    size_t index = 1;
    for (size_t i = 0, steps = settings_.propagation_steps; i < steps; ++i)
        index ^= 1;
    output_render_target_ = render_targets[index];

    return true;
}

void LPVMaterialSystem::destroy(Context& context)
{
    destroy_pool_object(context.render_target_pool, occlusion_render_target_);
    destroy_pool_object(context.render_target_pool, propagation_render_target_);
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
    geometry_injection(render_context.draw_calls.add(), context, render_context, vpl_number);
    propagation(context, render_context);
}

void LPVMaterialSystem::injection(DrawCall& draw_call,
                                  Context& context, RenderContext& render_context,
                                  size_t vpl_number)
{
    vec3 scene_min, scene_max;
    render_context.aabb.min_max(scene_min, scene_max);
    float diagonal_length = (scene_max - scene_min).length() * 1.2f;
    mat4x4 lpv_transform = mat4x4::translation_matrix(-scene_min) * mat4x4::scaling_matrix(1.0f / diagonal_length);
    float cell_size = diagonal_length / settings_.size;

    UniformBuffer& uniform = context.uniform_pool.get(injection_uniform_);
    InjectionShaderData shader_data;
    shader_data.settings.set_x(static_cast<float>(settings_.size));
    shader_data.rsm_to_ws = rsm_data_.vp.inverted();
    shader_data.ws_to_lpv = lpv_transform;
    shader_data.settings.set_y(cell_size);
    shader_data.settings.set_z(settings_.occlusion_coeff);
    shader_data.settings.set_w(settings_.propagation_amp);
    shader_data.light_parameters = vec4(-rsm_data_.light_direction, 0.0f);
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

void LPVMaterialSystem::geometry_injection(DrawCall& draw_call, Context& context, RenderContext& render_context, size_t vpl_number)
{
    Material& material = context.materials[id()].get(geometry_injection_material_);
    material.textures[0] = gbuffer_.normal;
    material.textures[1] = gbuffer_.accumulator;
    material.textures[2] = gbuffer_.depth;

    draw_call.material.id = material.id;
    draw_call.material.material_system = id();

    draw_call.render_data.elements_number = vpl_number;
    draw_call.render_data.primitive = gpu_generated;

    draw_call.render_state = injection_render_state_;

    draw_call.render_target = occlusion_render_target_;

    draw_call.pass = 1;
    draw_call.command = &clear_command_;
}

void LPVMaterialSystem::propagation(Context& context, RenderContext& render_context)
{
    RenderTargetHandleType render_targets[2] = {propagation_render_target_, render_target_grid_};
    size_t render_target_index = 0;

    Material& material0 = context.materials[id()].get(propagation_material_[0]);
    Material& material1 = context.materials[id()].get(propagation_material_[1]);
    if (settings_.use_occlusion)
    {
        material0.shader_program = propagation_shader_with_occlusion_;
        material1.shader_program = propagation_shader_with_occlusion_;
    }
    else
    {
        material0.shader_program = propagation_shader_;
        material1.shader_program = propagation_shader_;
    }

    const size_t grid_samples = settings_.size * settings_.size * settings_.size;

    for (size_t i = 0, steps = settings_.propagation_steps; i < steps; ++i)
    {
        size_t material_index = render_target_index;
        DrawCall& draw_call = render_context.draw_calls.add();
        draw_call.material.id = propagation_material_[material_index];
        draw_call.material.material_system = id();

        draw_call.render_data.elements_number = grid_samples;
        draw_call.render_data.primitive = gpu_generated;

        draw_call.render_state = injection_render_state_;

        draw_call.render_target = render_targets[render_target_index];

        draw_call.pass = 2 + i;
        draw_call.command = &clear_command_;

        render_target_index ^= 1;
    }
}

void LPVMaterialSystem::output(Context& context, size_t unit, TextureInstance& texture) const
{
    RenderTarget& rt = context.render_target_pool.get(output_render_target_);
    rt.color_texture(texture, unit);
}

void LPVMaterialSystem::init_debug_views(Context& context)
{
    size_t debug_view_id = context.debug_views->add_view(string("LPV"));
    DebugViews::DebugView& view = context.debug_views->get_view(debug_view_id);
    view.add(string("occlusion"), 0.0f, 20.0f, &settings_.occlusion_coeff);
    view.add(string("amp"), 0.0f, 5.0f, &settings_.propagation_amp);
}

bool LPVResolveMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    if (!PosteffectMaterialSystemBase::init(context, material_system_context))
        return false;
    UberShader& default_ubershader = ubershader(context);
    damp_info_ = default_ubershader.info("DAMP");
    return true;
}

void LPVResolveMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    PosteffectMaterialSystemBase::update(context, scene_context, render_context);
    UberShader::Index ubershader_index;
    ubershader_index.set(damp_info_, settings_.damping ? 1 : 0);
    default_material(context).shader_program = ubershader(context).get(ubershader_index);
}

}
