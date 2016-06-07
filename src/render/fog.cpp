#include "render/fog.hpp"

#include "render/context.hpp"
#include "render/layouts.hpp"
#include "render/render_context.hpp"
#include "render/instances.hpp"
#include "render/renderer.hpp"
#include "render/utils/simple_meshes.hpp"
#include "render/scene_context.hpp"
#include "render/lpv_material_system.hpp"
#include "utils/global_log.hpp"
#include "debug/debug_views.hpp"

namespace mhe {

bool HeightFogMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    if (!PosteffectMaterialSystemBase::init(context, material_system_context))
    {
        ERROR_LOG("PosteffectMaterialSystemBase::init failed");
        return false;
    }

    pool_initializer<UniformPool> uniform_wr(context.uniform_pool);
    UniformBufferDesc uniform_desc;
    uniform_desc.size = sizeof(ShaderData);
    uniform_desc.update_type = uniform_buffer_normal;
    uniform_desc.unit = 1;
    if (!uniform_wr.object().init(uniform_desc))
    {
        ERROR_LOG("Uniform buffer initialization failed");
        destroy(context);
        return false;
    }

    pool_initializer<RenderStatePool> render_state_wr(context.render_state_pool);
    RenderStateDesc render_state_desc;
    render_state_desc.blend.enabled = true;
    render_state_desc.blend.srcmode = blend_src_alpha;
    render_state_desc.blend.dstmode = blend_src_inv_alpha;
    render_state_desc.depth.test_enabled = false;
    render_state_desc.depth.write_enabled = false;
    render_state_desc.rasterizer.cull = cull_none;
    if (!render_state_wr.object().init(render_state_desc))
    {
        ERROR_LOG("RenderState initialization failed");
        destroy(context);
        return false;
    }

    mesh_instance().instance_parts[0].render_state_id = render_state_wr.get();
    default_material(context).uniforms[1] = uniform_wr.get();

    uniform_buffer_id_ = uniform_wr.take();
    render_state_id_ = render_state_wr.take();
    return true;
}

void HeightFogMaterialSystem::destroy(Context& context)
{
    destroy_pool_object(context.render_state_pool, render_state_id_);
    destroy_pool_object(context.uniform_pool, uniform_buffer_id_);
    PosteffectMaterialSystemBase::destroy(context);
}

void HeightFogMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    ShaderData shader_data;
    shader_data.param1 = vec4(settings_.start, settings_.density, settings_.falloff, 0.0f);
    shader_data.fog_color = settings_.color;
    UniformBuffer& uniform = context.uniform_pool.get(uniform_buffer_id_);
    uniform.update(shader_data);

    PosteffectMaterialSystemBase::update(context, scene_context, render_context);
}

bool ExponentialShadowMap::DownsampleCommand::init(const Settings& settings)
{
    set_stages(render_stage_before_render_target_setup);
    settings_ = settings;
    return true;
}

bool ExponentialShadowMap::DownsampleCommand::execute_impl(Context& context, RenderStage current_stage)
{
    ComputeCallExplicit compute_call;
    prepare_draw_call(compute_call);
    compute_call.uniforms[2] = &context.uniform_pool.get(settings_.uniform);
    compute_call.shader_program = &context.shader_pool.get(settings_.shader);
    compute_call.images[0] = &context.texture_pool.get(settings_.source_texture.id);
    compute_call.images[1] = &context.texture_pool.get(settings_.dest_texture.id);
    compute_call.image_access[0] = access_readonly;
    compute_call.image_access[1] = access_writeonly;
    compute_call.workgroups_number = settings_.threads_number;
    context.driver.execute(context, &compute_call, 1);
    context.driver.memory_barrier(memory_barrier_all);

    return true;
}

bool ExponentialShadowMap::init(Context& context, const MaterialSystemContext& material_system_context)
{
    set_layout(FullscreenLayout::handle);
    if (!init_default(context, material_system_context))
    {
        ERROR_LOG("ExponentialShadowMap base initialization failed");
        return false;
    }

    const string& downsample_shader = material_system_context.options.get<string>("downsample_shader");
    ASSERT(!downsample_shader.empty(), "Downsample shader name must not be empty");
    esm_size_ = material_system_context.options.get<size_t>("size");
    // render target
    RenderTargetDesc render_target_desc;
    render_target_desc.color_address_mode[0] = texture_clamp;
    render_target_desc.color_datatype[0] = format_default;
    render_target_desc.color_filter[0] = texture_filter_nearest;
    render_target_desc.color_format[0] = format_r32f;
    render_target_desc.color_targets = 1;
    render_target_desc.height = esm_size_;
    render_target_desc.target = rt_readwrite;
    render_target_desc.texture_type = texture_2d;
    render_target_desc.use_depth = false;
    render_target_desc.use_stencil = false;
    render_target_desc.width = esm_size_;
    RenderTarget& render_target = create_and_get(context.render_target_pool);
    bool res = render_target.init(context, render_target_desc);
    ASSERT(res, "Render target initialization failed");
    resolved_shadow_rt_ = render_target.id();
    // depth resolving
    MaterialSystemContext resolve_material_system_context;
    resolve_material_system_context.instance_name = string("esm_depth_copy");
    resolve_material_system_context.priority = priority() - 1;
    resolve_material_system_context.shader_name = string("posteffect_copy");
    resolve_material_system_context.material_instances_number = 1;
    context.initialization_parameters.add(string("posteffect_simple"), resolve_material_system_context);
    depth_copy_material_system_ = create<PosteffectSimpleMaterialSystem>(context, string("posteffect_simple"), string("esm_depth_copy"));
    ASSERT(depth_copy_material_system_ != nullptr, "Couldn't create a material system for depth copying");

    Shader shader;
    res = context.shader_manager.get(shader, downsample_shader);
    ASSERT(res, "Downsample shader loading failed");
    downsample_shader_program_id_ = context.ubershader_pool.get(shader.shader_program_handle).get_default();
    ShaderProgram& downsample_shader_program = context.shader_pool.get(downsample_shader_program_id_);
    cs_threads_number_ = downsample_shader_program.variable_value<size_t>("THREADS_NUMBER");

    UniformBufferDesc uniform_buffer_desc;
    uniform_buffer_desc.size = sizeof(ShaderData);
    uniform_buffer_desc.unit = 2;
    uniform_buffer_desc.update_type = uniform_buffer_normal;
    UniformBuffer& uniform = create_and_get(context.uniform_pool);
    res = uniform.init(uniform_buffer_desc);
    ASSERT(res, "Uniform initialization failed");
    uniform_id_ = uniform.id();

    // mesh
    res = utils::create_fullscreen_quad(mesh_instance_, context);
    ASSERT(res, "create_fullscreen_quad failed");
    Material& material = create_and_get(context.materials[id()]);
    material.shader_program = ubershader(context).get_default();
    mesh_instance_.instance_parts[0].material.id = material.id;
    mesh_instance_.instance_parts[0].material.material_system = id();

    RenderStateDesc render_state_desc;
    render_state_desc.blend.enabled = false;
    render_state_desc.depth.test_enabled = false;
    render_state_desc.depth.write_enabled = false;
    render_state_desc.rasterizer.cull = cull_none;
    RenderState& render_state = create_and_get(context.render_state_pool);
    res = render_state.init(render_state_desc);
    ASSERT(res, "render state initialization failed");
    mesh_instance_.instance_parts[0].render_state_id = render_state.id();

    light_instance_id_ = LightInstance::invalid_id;

    // blur
    blur_material_system_ = new BlurMaterialSystem;
    MaterialSystemContext blur_material_system_context;
    blur_material_system_context.instance_name = "esm_blur";
    blur_material_system_context.priority = material_system_context.priority;
    blur_material_system_context.material_instances_number = 2;
    blur_material_system_context.shader_name = material_system_context.options.get<string>("blur_shader");
    context.material_systems.add(blur_material_system_);
    context.materials[blur_material_system_->id()].resize(2);
    if (!blur_material_system_->init(context, blur_material_system_context))
    {
        ERROR_LOG("Can't initialize a BlurMaterialSystem for ESM");
        destroy(context);
        return false;
    }
    blur_material_system_->settings().quality = BlurMaterialSystem::quality_ultra;

    return true;
}

void ExponentialShadowMap::destroy(Context& context)
{
    destroy_pool_object(context.render_state_pool, mesh_instance_.instance_parts[0].render_state_id);
    destroy_mesh_instance(mesh_instance_, context);
    destroy_pool_object(context.render_target_pool, resolved_shadow_rt_);
    destroy_pool_object(context.texture_pool, downsampled_texture_.id);
}

void ExponentialShadowMap::setup(Context &context, SceneContext &scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void ExponentialShadowMap::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    // working with first directional light only
    LightInstance* directional_light_instance = nullptr;
    for (size_t i = 0; i < render_context.lights_number; ++i)
    {
        if (render_context.lights[i].light.type() == Light::directional)
        {
            directional_light_instance = &render_context.lights[i];
            break;
        }
    }

    if (directional_light_instance == nullptr)
        return;

    light_instance_id_ = directional_light_instance->id;

    const ShadowInfo* shadow_info = directional_light_instance->light.shadow_info();
    if (shadow_info == nullptr) return;
    if (!is_handle_valid(downsampled_texture_.id))
        init_downsampling(context, shadow_info);

    depth_copy_material_system_->set_priority(priority() - 1);
    depth_copy_material_system_->setup_draw_calls(context, scene_context, render_context);

    ShaderData shader_data;
    shader_data.settings.set_x(settings_.exponent);
    context.uniform_pool.get(uniform_id_).update(shader_data);

    // update material
    Material& material = context.materials[id()].get(mesh_instance_.instance_parts[0].material.id);
    material.uniforms[0] = render_context.main_camera.percamera_uniform;
    material.uniforms[1] = directional_light_instance->uniform_id;
    material.uniforms[2] = uniform_id_;
    material.textures[0] = downsampled_texture_;
    material.textures[3] = context.renderer->scene_depth_buffer();

    // blur
    DrawCall& dc = render_context.draw_calls.add();
    DrawCall& dc2 = render_context.draw_calls.add();
    blur_material_system_->setup_draw_calls(context, scene_context, &dc, 2, render_context);
    dc.command = &downsample_command_;
    dc.pass = 0;
    dc2.pass = 1;
}

TextureInstance ExponentialShadowMap::shadowmap_texture() const
{
    return blur_material_system_->output(0);
}

void ExponentialShadowMap::init_downsampling(Context& context, const ShadowInfo* shadow_info)
{
    Texture& shadowmap_texture = context.texture_pool.get(shadow_info->shadowmap.id);
    size_t width = shadowmap_texture.width();
    size_t height = shadowmap_texture.height();

    size_t downsampled_height = esm_size_;
    size_t downsampled_width = esm_size_ * shadow_info->cascades_number;

    TextureDesc texture_desc;
    texture_desc.address_mode_r = texture_clamp;
    texture_desc.address_mode_t = texture_clamp;
    texture_desc.address_mode_s = texture_clamp;
    texture_desc.datatype = format_default;
    texture_desc.format = format_r32f;
    texture_desc.mag_filter = texture_filter_linear;
    texture_desc.min_filter = texture_filter_linear;
    texture_desc.type = texture_2d;

    size_t steps = 0;
    texture_desc.height = static_cast<size_t>(height * 0.25f);
    texture_desc.width = static_cast<size_t>(width * 0.25f);
    Texture& texture = create_and_get(context.texture_pool);
    bool res = texture.init(texture_desc, nullptr, 0);
    ASSERT(res, "Couldn't initialize a texture");
    downsampled_texture_.id = texture.id();

    depth_copy_material_system_->set_input(0, shadow_info->shadowmap);
    RenderTargetDesc rt_desc;
    rt_desc.color_address_mode[0] = texture_clamp;
    rt_desc.color_datatype[0] = format_default;
    rt_desc.color_filter[0] = texture_filter_nearest;
    rt_desc.color_format[0] = format_r32f;
    rt_desc.color_targets = 1;
    rt_desc.height = height;
    rt_desc.target = rt_readwrite;
    rt_desc.texture_type = texture_2d;
    rt_desc.use_depth = false;
    rt_desc.use_stencil = false;
    rt_desc.width = width;
    RenderTarget& render_target = create_and_get(context.render_target_pool);
    res = render_target.init(context, rt_desc);
    ASSERT(res, "Couldn't create a R32F depth render target");
    depth_copy_material_system_->set_render_target(render_target.id(), context);

    DownsampleCommand::Settings settings;
    render_target.color_texture(settings.source_texture, 0);
    settings.dest_texture = downsampled_texture_;
    settings.shader = downsample_shader_program_id_;
    settings.uniform = uniform_id_;
    settings.threads_number = uivec3(texture_desc.width / cs_threads_number_, texture_desc.height / cs_threads_number_, 1);
    downsample_command_.init(settings);

    blur_material_system_->PosteffectMaterialSystemBase::create_output(context, 0, texture_desc.width, texture_desc.height, format_r32f);
    blur_material_system_->set_input(0, downsampled_texture_);
}

TextureInstance ExponentialShadowMap::shadow_texture(Context& context) const
{
    TextureInstance texture;
    size_t size = context.render_target_pool.get(resolved_shadow_rt_).color_texture(texture, 0);
    ASSERT(size == 1, "Invalid render target");
    return texture;
}

bool VolumetricFogMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    if (!init_default(context, material_system_context))
    {
        ERROR_LOG("VolumetricFogMaterialSystem initialization failed");
        return false;
    }

    volume_size_.x() = material_system_context.options.get<size_t>("volume_width");
    volume_size_.y() = material_system_context.options.get<size_t>("volume_height");
    volume_size_.z() = material_system_context.options.get<size_t>("volume_depth");
    const string& propagation_shader_name = material_system_context.options.get<string>("propagation_shader");
    ASSERT(!propagation_shader_name.empty(), "Propagation shader name must not be empty");
    Shader propagation_ubershader;
    context.shader_manager.get(propagation_ubershader, propagation_shader_name);
    propagation_shader_ = context.ubershader_pool.get(propagation_ubershader.shader_program_handle).get_default();

    TextureDesc texture_desc;
    texture_desc.address_mode_r = texture_clamp_to_edge;
    texture_desc.address_mode_s = texture_clamp_to_edge;
    texture_desc.address_mode_t = texture_clamp_to_edge;
    texture_desc.anisotropic_level = 0.0f;
    texture_desc.datatype = format_default;
    texture_desc.depth = volume_size_.z();
    texture_desc.format = format_rgba16f;
    texture_desc.height = volume_size_.y();
    texture_desc.mag_filter = texture_filter_linear;
    texture_desc.min_filter = texture_filter_linear;
    texture_desc.mips = 0;
    texture_desc.type = texture_3d;
    texture_desc.width = volume_size_.x();

    for (int i = 0; i < 2; ++i)
    {
        Texture& texture = create_and_get(context.texture_pool);
        if (!texture.init(texture_desc, nullptr, 0))
        {
            ERROR_LOG("Couldn't create a volume texture");
            destroy(context);
            return false;
        }
        volume_textures_[i].id = texture.id();
    }

    UniformBuffer& uniform_buffer = create_and_get(context.uniform_pool);
    UniformBufferDesc uniform_buffer_desc;
    uniform_buffer_desc.size = sizeof(ShaderData);
    uniform_buffer_desc.unit = 1;
    uniform_buffer_desc.update_type = uniform_buffer_normal;
    if (!uniform_buffer.init(uniform_buffer_desc))
    {
        ERROR_LOG("Couldn't create a uniform buffer");
        destroy(context);
        return false;
    }

    uniform_id_ = uniform_buffer.id();

    list_of_commands_.add_command(&compute_call_command_);
    list_of_commands_.add_command(&propagation_command_);

    return true;
}

void VolumetricFogMaterialSystem::destroy(Context& context)
{
    destroy_pool_object(context.uniform_pool, uniform_id_);
    destroy_pool_object(context.texture_pool, volume_textures_[0].id);
    destroy_pool_object(context.texture_pool, volume_textures_[1].id);
}

void VolumetricFogMaterialSystem::setup(Context &context, SceneContext &scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void VolumetricFogMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    // can initialize this uniform here
    ShaderData shader_data;
    shader_data.volume_size.set(static_cast<float>(volume_size_.x()),
                                static_cast<float>(volume_size_.y()),
                                static_cast<float>(volume_size_.z()), settings_.range);
    shader_data.fog_color = settings_.color;
    shader_data.fog_settings.set(settings_.density, settings_.falloff, settings_.scattering_coeff, settings_.light_brightness);
    UniformBuffer& uniform_buffer = context.uniform_pool.get(uniform_id_);
    uniform_buffer.update(shader_data);

    if (!is_handle_valid(light_instance_id_))
        return;
    LightInstance& light_instance = scene_context.light_pool.get(light_instance_id_);

    // fog initialization
    ComputeCallExplicit& compute_call = compute_call_command_.compute_call();
    prepare_draw_call(compute_call);
    compute_call.images[0] = nullptr;
    compute_call.images[1] = nullptr;
    compute_call.images[2] = &context.texture_pool.get(volume_textures_[0].id);
    compute_call.textures[1] = &context.texture_pool.get(shadow_texture_.id);
    compute_call.image_access[0] = access_readonly;
    compute_call.image_access[1] = access_readonly;
    compute_call.image_access[2] = access_writeonly;
    compute_call.uniforms[0] = &context.uniform_pool.get(render_context.main_camera.percamera_uniform);
    compute_call.uniforms[1] = &uniform_buffer;
    compute_call.uniforms[2] = &context.uniform_pool.get(light_instance.uniform_id);
    compute_call.uniforms[3] = &context.uniform_pool.get(esm_uniform_id_);

    LPVMaterialSystem* lpv_material_system = context.renderer->gi_system().lpv_material_system();
    if (lpv_material_system != nullptr)
    {
        for (int i = 0; i < 3; ++i)
        {
            TextureInstance gi_texture;
            lpv_material_system->output(context, i, gi_texture);
            ASSERT(is_handle_valid(gi_texture.id), "Invalid GI texture");
            compute_call.textures[3 + i] = &context.texture_pool.get(gi_texture.id);
            compute_call.image_access[3 + i] = access_readonly;
        }
        UniformBuffer::IdType gi_uniform = lpv_material_system->injection_settings_uniform();
        ASSERT(is_handle_valid(gi_uniform), "Invalid GI uniform");
        compute_call.uniforms[4] = &context.uniform_pool.get(gi_uniform);
    }

    UberShader& uber_shader = ubershader(context);
    UberShader::Index ubershader_index;
    ubershader_index.set(uber_shader.info("LPV"), lpv_material_system != nullptr ? 1 : 0);
    compute_call.shader_program = &context.shader_pool.get(uber_shader.get(ubershader_index));

    size_t threads_number = compute_call.shader_program->variable_value<size_t>(string("THREADS_NUMBER"));
    ASSERT(volume_size_.x() % threads_number == 0 && volume_size_.y() % threads_number == 0 &&
        volume_size_.z() % threads_number == 0, "Volume size must be a multiplier of " << threads_number);
    compute_call.workgroups_number = volume_size_ / threads_number;
    compute_call.barrier = memory_barrier_all;

    // fog propagation
    ComputeCallExplicit& propagation_compute_call = propagation_command_.compute_call();
    prepare_draw_call(propagation_compute_call);
    propagation_compute_call.images[0] = compute_call.images[2];
    propagation_compute_call.images[1] = &context.texture_pool.get(volume_textures_[1].id);
    propagation_compute_call.image_access[0] = access_readonly;
    propagation_compute_call.image_access[1] = access_writeonly;
    propagation_compute_call.uniforms[1] = compute_call.uniforms[1];
    propagation_compute_call.shader_program = &context.shader_pool.get(propagation_shader_);
    threads_number = propagation_compute_call.shader_program->variable_value<size_t>(string("THREADS_NUMBER"));
    ASSERT(volume_size_.x() % threads_number == 0 && volume_size_.y() % threads_number == 0,
           "Volume size must be a multiplier of " << threads_number);
    propagation_compute_call.workgroups_number = uivec3(volume_size_.xy() / threads_number, 1);
    propagation_compute_call.barrier = memory_barrier_all;

    DrawCall& draw_call = render_context.draw_calls.add();
    draw_call.material.material_system = id();
    draw_call.command = &list_of_commands_;
}

void VolumetricFogMaterialSystem::init_debug_views(Context& context)
{
    size_t view_id = context.debug_views->add_view(string("Fog"));
    DebugViews::DebugView& view = context.debug_views->get_view(view_id);
    view.add(string("density"), 0.0f, 5.0f, &settings_.density);
    view.add(string("scattering"), 0.0f, 1.0f, &settings_.scattering_coeff);
    view.add(string("falloff"), 0.0f, 1.0f, &settings_.falloff);
    view.add(string("range"), 50.0f, 5000.0f, &settings_.range);
    view.add(string("light_brightness"), 0.0f, 1.0f, &settings_.light_brightness);
}

}
