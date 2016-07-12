#include "render/posteffect_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/instances.hpp"
#include "render/utils/simple_meshes.hpp"
#include "render/render_context.hpp"
#include "utils/strutils.hpp"
#include "debug/debug_views.hpp"
#include "debug/gpu_profiler.hpp"

namespace mhe {

bool PosteffectDebugMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    texture_type_mask_ = 0;
    textures_number_ = 0;
    set_layout(FullscreenLayout::handle);

    if (!context.shader_manager.get(shader(), material_system_context.shader_name))
    {
        ERROR_LOG("Can't get shader:" << material_system_context.shader_name);
        return false;
    }

    return init_mesh(context, material_system_context);
}

bool PosteffectDebugMaterialSystem::init_mesh(Context& context, const MaterialSystemContext& material_system_context)
{
    bool overdraw = material_system_context.options.get<bool>("overdraw");

    int window_width = context.window_system.width();
    int window_height = context.window_system.height();
    rect<int> viewports[max_textures_number];
    viewports[0] = rect<int>(0, window_height / 2, window_width / 2, window_height / 2);
    viewports[1] = rect<int>(window_width / 2, window_height / 2, window_width / 2, window_height / 2);
    viewports[2] = rect<int>(0, 0, window_width / 2, window_height / 2);
    viewports[3] = rect<int>(window_width / 2, 0, window_width / 2, window_height / 2);

    for (size_t i = 0; i < max_textures_number; ++i)
    {
        if (!init_mesh_instance(context, mesh_[i], viewports[i], overdraw))
        {
            ERROR_LOG("Can't initialize mesh for PosteffectDebug");
            return false;
        }
    }

    if (!init_mesh_instance(context, fullscreen_mesh_, rect<int>(0, 0, window_width, window_height), overdraw))
    {
        ERROR_LOG("Can't initialize mesh for PosteffectDebug");
        return false;
    }

    return true;
}

bool PosteffectDebugMaterialSystem::init_mesh_instance(Context& context, MeshInstance& mesh_instance, const rect<int>& viewport, bool overdraw)
{
    mesh_instance.mesh.parts.resize(1);
    mesh_instance.instance_parts.resize(1);
    if (!utils::create_fullscreen_quad(mesh_instance, context))
    {
        ERROR_LOG("Can't create fullscreen quad");
        return false;
    }
    mesh_instance.mesh.parts[0].render_data.layout = layout();

    RenderState& render_state = create_and_get(context.render_state_pool);

    mesh_instance.instance_parts[0].render_state_id = render_state.id();
    RenderStateDesc desc;
    desc.blend.enabled = true;
    desc.viewport.viewport = viewport;
    if (!overdraw)
    {
        desc.blend.srcmode = blend_src_alpha;
        desc.blend.dstmode = blend_src_inv_alpha;
    }
    else
    {
        desc.blend.srcmode = blend_one;
        desc.blend.dstmode = blend_zero;
    }
    desc.depth.test_enabled = false;
    desc.depth.write_enabled = false;
    desc.rasterizer.cull = cull_none;
    render_state.init(desc);

    Material& material = create_and_get(context.materials[id()]);
    material.shader_program = default_program(context).id();
    mesh_instance.instance_parts[0].material.material_system = id();
    mesh_instance.instance_parts[0].material.id = material.id;

    return true;
}

void PosteffectDebugMaterialSystem::destroy(Context& context)
{
    destroy_mesh_instance(fullscreen_mesh_, context);
    for (size_t i = 0; i < max_textures_number; ++i)
        destroy_mesh_instance(mesh_[i], context);
}

void PosteffectDebugMaterialSystem::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void PosteffectDebugMaterialSystem::update(Context& context, SceneContext& /*scene_context*/, RenderContext& render_context)
{
    UberShader& shader = ubershader(context);
    UberShader::Index ubershader_index;
    const UberShader::Info& info = shader.info("RENDER_DEPTH");
    ubershader_index.set(info, 1);

    clear_command_.reset();

    if (textures_number_ <= 1) // can draw fullscreen. textures_number_ can be 0
    {
        Material& material = context.materials[id()].get(fullscreen_mesh_.instance_parts[0].material.id);
        size_t texture_index = textures_[0].id == Texture::invalid_id ? max_textures_number - 1 : 0;
        material.textures[0] = textures_[texture_index];
        if (texture_type_mask_ & (1 << texture_index))
            material.shader_program = shader.get(ubershader_index);
        else material.shader_program = shader.get_default();
        setup_draw_call(render_context.draw_calls.add(), fullscreen_mesh_.instance_parts[0], fullscreen_mesh_.mesh.parts[0], mhe::default_render_target, &clear_command_);
    }
    else
    {
        for (size_t j = 0; j < max_textures_number; ++j)
        {
            if (textures_[j].id == Texture::invalid_id) continue;
            Material& material = context.materials[id()].get(mesh_[j].instance_parts[0].material.id);
            material.textures[0] = textures_[j];

            if (texture_type_mask_ & (1 << j))
                material.shader_program = shader.get(ubershader_index);

            setup_draw_call(render_context.draw_calls.add(), mesh_[j].instance_parts[0], mesh_[j].mesh.parts[0], mhe::default_render_target, &clear_command_);
        }
    }
}

void PosteffectDebugMaterialSystem::set_render_target(const RenderTarget& render_target)
{
    for (size_t i = 0; i < max_textures_number; ++i)
        textures_[i].id = Texture::invalid_id;
    size_t color_textures_number = render_target.color_textures(textures_);
    size_t depth_textures_number = render_target.depth_texture(textures_[max_textures_number - 1]);
    texture_type_mask_ = depth_textures_number == 0 ? 0 : (1 << 3);
    textures_number_ = color_textures_number + depth_textures_number;
}

PosteffectMaterialSystemBase::PosteffectMaterialSystemBase(const char* name) :
    default_render_target_(mhe::default_render_target),
    inputs_number_(0), outputs_number_(0), profile_command_(name),
    framebuffer_input_(invalid_index)
{}

bool PosteffectMaterialSystemBase::init(Context& context, const MaterialSystemContext& material_system_context)
{
    set_layout(FullscreenLayout::handle);

    if (!context.shader_manager.get(shader(), material_system_context.shader_name))
    {
        ERROR_LOG("Can't get shader:" << material_system_context.shader_name);
        return false;
    }

    buffers_.fill(InvalidHandle<ShaderStorageBufferHandleType>::id);
    uniforms_.fill(InvalidHandle<UniformBufferHandleType>::id);

    inputs_number_shader_info_ = ubershader(context).info("INPUTS");

    return init_mesh(context, material_system_context);
}

void PosteffectMaterialSystemBase::destroy(Context& context)
{
    destroy_mesh_instance(mesh_, context);
}

void PosteffectMaterialSystemBase::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void PosteffectMaterialSystemBase::prepare_draw_call(DrawCall& draw_call, Context& context, SceneContext& /*scene_context*/, const RenderContext& render_context,
    RenderTargetHandleType render_target_id)
{
    list_of_commands_.clear();
    clear_command_.reset();
    if (framebuffer_input_ != invalid_index) // current framebuffer as input
    {
        copy_framebuffer_command_.set_texture(&context.texture_pool.get(inputs_[framebuffer_input_].id));
        list_of_commands_.add_command(&copy_framebuffer_command_);
    }

    if (render_target_id != mhe::default_render_target &&
        render_target_id != RenderTarget::invalid_id)
        list_of_commands_.add_command(&clear_command_);

    list_of_commands_.add_command(&profile_command_);
        
    RenderCommand* command = list_of_commands_.empty() ? nullptr : &list_of_commands_;

    UberShader::Index ubershader_index;
    ubershader_index.set(inputs_number_shader_info_, inputs_number_ - 1);
    UberShader& shader = ubershader(context);
    
    Material& material = context.materials[id()].get(mesh_.instance_parts[0].material.id);
    for (size_t j = 0; j < max_textures_number; ++j)
        material.textures[j] = inputs_[j];
    material.uniforms[0] = render_context.main_camera.percamera_uniform;
    for (size_t j = 0; j < max_buffers; ++j)
    {
        if (uniforms_[j] != UniformBuffer::invalid_id)
            material.uniforms[j] = uniforms_[j];
        if (buffers_[j] != InvalidHandle<ShaderStorageBufferHandleType>::id)
            material.shader_storage_buffers[j] = buffers_[j];
    }
    material.shader_program = shader.get(ubershader_index);
    setup_draw_call(draw_call, mesh_.instance_parts[0], mesh_.mesh.parts[0], render_target_id, command);
    draw_call.pass = 0;
}

void PosteffectMaterialSystemBase::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    prepare_draw_call(render_context.draw_calls.add(), context, scene_context, render_context, default_render_target_);
}

bool PosteffectMaterialSystemBase::init_mesh(Context& context, const MaterialSystemContext& material_system_context)
{
    mesh_.mesh.parts.resize(1);
    mesh_.instance_parts.resize(1);
    if (!utils::create_fullscreen_quad(mesh_, context))
    {
        ERROR_LOG("Can't create fullscreen quad");
        return false;
    }
    mesh_.mesh.parts[0].render_data.layout = layout();

    RenderState& render_state = create_and_get(context.render_state_pool);

    bool use_blend = material_system_context.options.get<bool>(string("blend"));

    mesh_.instance_parts[0].render_state_id = render_state.id();
    RenderStateDesc desc;
    desc.blend.enabled = use_blend;
    desc.blend.srcmode = blend_src_alpha;
    desc.blend.dstmode = blend_src_inv_alpha;
    desc.depth.test_enabled = false;
    desc.depth.write_enabled = false;
    desc.rasterizer.cull = cull_none;
    render_state.init(desc);

    // outputs
    // TODO: add support for render target with multiply outputs
    const string& outputs_str = material_system_context.options.get<string>(string("outputs"));
    const std::vector<string>& outputs_scale = utils::split(outputs_str, string(","));
    if (!outputs_scale.empty())
    {
        create_output(render_state.id(), context, 0, 0, 0, types_cast<float>(outputs_scale[0]), format_rgba);
    }

    Material& material = create_and_get(context.materials[id()]);
    material.shader_program = default_program(context).id();
    mesh_.instance_parts[0].material.material_system = id();
    mesh_.instance_parts[0].material.id = material.id;
    
    return true;
}

bool PosteffectMaterialSystemBase::init_screen_input(Context& context, size_t index, uint8_t render_stage)
{
    ASSERT(input(index).id == Texture::invalid_id, "Invalid posteffects description");
    TextureDesc desc;
    desc.datatype = format_ubyte;
    desc.format = format_rgba;
    inputs_[index] = context.render_target_manager.create(context, desc, 1.0f);
    framebuffer_input_ = index;
    copy_framebuffer_command_.set_stages(render_stage);
    return true;
}

bool PosteffectMaterialSystemBase::create_output(Context& context, size_t index, float scale, int format)
{
    ASSERT(output(index).id == Texture::invalid_id, "create_output() - trying to create a new output when old output still exists");
    ASSERT(default_render_target_ == mhe::default_render_target || default_render_target_ == RenderTarget::invalid_id,
        "RenderTarget has been set already");
    return create_output(mesh_.instance_parts[0].render_state_id, context, index, 0, 0, scale, format);
}

bool PosteffectMaterialSystemBase::create_output(Context& context, size_t index, size_t width, size_t height, int format)
{
    ASSERT(output(index).id == Texture::invalid_id, "create_output() - trying to create a new output when old output still exists");
    ASSERT(default_render_target_ == mhe::default_render_target || default_render_target_ == RenderTarget::invalid_id,
        "RenderTarget has been set already");
    return create_output(mesh_.instance_parts[0].render_state_id, context, index, width, height, 0.0f, format);
}

void PosteffectMaterialSystemBase::fill_render_target_desc(RenderTargetDesc& desc, size_t width, size_t height, int format) const
{
    desc.target = rt_readwrite;
    desc.use_depth = false;
    desc.use_stencil = false;
    desc.color_targets = 1;
    desc.color_datatype[0] = format_default;
    desc.color_format[0] = format != format_max ? format : format_rgba;
    desc.width = width;
    desc.height = height;
}

bool PosteffectMaterialSystemBase::create_output(RenderStateHandleType render_state_id, Context& context, size_t index,
    size_t width, size_t height, float scale, int format)
{
    RenderTargetDesc render_target_desc;
    fill_render_target_desc(render_target_desc, width, height, format);
    RenderTarget& render_target = context.render_target_manager.create(context, render_target_desc, scale);
    const TextureInstance* ids = nullptr;
    render_target.color_textures(&ids);
    set_output(index, ids[0]);

    default_render_target_ = render_target.id();

    if (scale != 1.0f)
    {
        RenderState& render_state = context.render_state_pool.get(render_state_id);
        ViewportDesc desc;
        desc.viewport.set(0, 0, render_target_desc.width, render_target_desc.height);
        render_state.update_viewport(desc);
    }

    return true;
}

Material& PosteffectMaterialSystemBase::default_material(Context& context)
{
    return context.materials[id()].get(mesh_.instance_parts[0].material.id);
}

RenderTargetHandleType PosteffectMaterialSystemBase::default_render_target() const
{
    return default_render_target_;
}

void PosteffectMaterialSystemBase::set_render_target(RenderTargetHandleType render_target_id, Context& context)
{
    default_render_target_ = render_target_id;
    RenderTarget& render_target = context.render_target_pool.get(render_target_id);
    const TextureInstance* color_textures = nullptr;
    size_t outputs_number = render_target.color_textures(&color_textures);
    if (outputs_number == 0) return;
    for (size_t i = 0; i < outputs_number; ++i)
        set_output(i, color_textures[i]);
    // TODO: also add render target's depth
    RenderState& render_state = context.render_state_pool.get(mesh_instance().instance_parts[0].render_state_id);
    ViewportDesc viewport_desc;
    viewport_desc.viewport.set(0, 0, render_target.width(), render_target.height());
    render_state.update_viewport(viewport_desc);
}

bool SSRMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    if (!PosteffectMaterialSystemBase::init(context, material_system_context))
    {
        ERROR_LOG("SSR: can't initialize PosteffectMaterialSystemBase");
        return false;
    }

    UniformBuffer& uniform = create_and_get(context.uniform_pool);
    UniformBufferDesc desc;
    desc.size = sizeof(SSRShaderData);
    desc.unit = 1;
    desc.update_type = uniform_buffer_normal;
    if (!uniform.init(desc))
    {
        ERROR_LOG("Can't initialize SSR uniform");
        return false;
    }

    ssr_uniform_ = uniform.id();
    default_material(context).uniforms[1] = uniform.id();

    ssr_shader_data_.ssrdata[0] = vec4(10.0f, 1000.0f, 0.5f, 0.5f);
    ssr_shader_data_.ssrdata[1] = vec4(10.0f, 1000.0f, 0.5f, 20.0f);
    ssr_shader_data_.ssrdata[2] = vec4(0.9f, 0.0f, 0.0f, 0.0f);

    probes_info_ = ubershader(context).info("PROBES");

    return true;
}

void SSRMaterialSystem::destroy(Context& context)
{
    destroy_pool_object(context.uniform_pool, ssr_uniform_);
    PosteffectMaterialSystemBase::destroy(context);
}

void SSRMaterialSystem::init_debug_views(Context& context)
{
    size_t debug_view_id = context.debug_views->add_view("SSR");
    DebugViews::DebugView& view = context.debug_views->get_view(debug_view_id);
    view.add("probes", 0.0f, 30.0f, &ssr_shader_data_.ssrdata[0].x());
    view.add("max_probe_distance", 100.0f, 2000.0f, &ssr_shader_data_.ssrdata[0].y());
    view.add("min_probe_distance", 0.0f, 10.0f, &ssr_shader_data_.ssrdata[0].z());
    view.add("max error", 0.0f, 10.0f, &ssr_shader_data_.ssrdata[0].w());
    view.add("start distance", 0.0f, 500.0f, &ssr_shader_data_.ssrdata[1].x());
    view.add("end distance", 600.0f, 2000.0f, &ssr_shader_data_.ssrdata[1].y());
    view.add("intensity", 0.0f, 5.0f, &ssr_shader_data_.ssrdata[1].z());
    view.add("fade distance", 0.0f, 100.0f, &ssr_shader_data_.ssrdata[1].w());
    view.add("UV fade", 0.0f, 1.0f, &ssr_shader_data_.ssrdata[2].x());
}

void SSRMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    UniformBuffer& uniform = context.uniform_pool.get(ssr_uniform_);
    uniform.update(ssr_shader_data_);

    PosteffectMaterialSystemBase::update(context, scene_context, render_context);
    // TODO: need to rewrite it
    if (settings_.use_probes && input(probes_texture_unit).id != Texture::invalid_id)
    {
        UberShader::Index index;
        index.set(probes_info_, 1);
        default_material(context).shader_program = ubershader(context).get(index);
    }
}

bool BlurMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    if (!PosteffectMaterialSystemBase::init(context, material_system_context))
        return false;
    pass_info_ = ubershader(context).info("PASS");
    quality_info_ = ubershader(context).info("QUALITY");

    UniformBuffer& uniform_buffer = create_and_get(context.uniform_pool);
    UniformBufferDesc uniform_buffer_desc;
    uniform_buffer_desc.size = sizeof(ShaderData);
    uniform_buffer_desc.unit = 1;
    uniform_buffer_desc.update_type = uniform_buffer_normal;
    if (!uniform_buffer.init(uniform_buffer_desc))
    {
        ERROR_LOG("Can't initialize a UniformBuffer");
        return false;
    }
    uniform_ = uniform_buffer.id();

    MeshInstance& first_pass_mesh = mesh_instance();
    context.materials[id()].get(first_pass_mesh.instance_parts[0].material.id).uniforms[1] = uniform_;
    second_pass_mesh_ = first_pass_mesh;

    Material& material = create_and_get(context.materials[id()]);
    second_pass_mesh_.instance_parts[0].material.id = material.id;
    material.uniforms[1] = uniform_;

    return true;
}

void BlurMaterialSystem::destroy(Context& context)
{
    destroy_pool_object(context.uniform_pool, uniform_);
    context.render_target_manager.destroy(output_rt_, context);
    PosteffectMaterialSystemBase::destroy(context);
}

bool BlurMaterialSystem::create_output(RenderStateHandleType render_state_id, Context& context, size_t index,
    size_t width, size_t height, float scale, int format)
{
    if (!PosteffectMaterialSystemBase::create_output(render_state_id, context, index, width, height, scale, format))
        return false;

    RenderTargetDesc render_target_desc;
    fill_render_target_desc(render_target_desc, width, height, format);
    RenderTarget& render_target = context.render_target_manager.create(context, render_target_desc, scale);
    output_rt_ = render_target.id();

    // current output (first pass) as input for second pass
    Material& second_pass_material = context.materials[id()].get(second_pass_mesh_.instance_parts[0].material.id);
    second_pass_material.textures[0] = output(0);

    TextureInstance texture;
    render_target.color_texture(texture, 0);
    set_output(0, texture);

    return true;
}

void BlurMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    DrawCall& draw_call = render_context.draw_calls.add();
    render_context.draw_calls.add();
    setup_draw_calls(context, scene_context, &draw_call, 2, render_context);
}

void BlurMaterialSystem::setup_draw_calls(Context& context, SceneContext& scene_context, DrawCall* draw_calls, size_t draw_calls_number,
    const RenderContext& render_context)
{
    ASSERT(draw_calls_number >= 2, "At least two DrawCalls are required");
    ShaderData data;
    data.params.set_x(settings_.size);
    context.uniform_pool.get(uniform_).update(data);

    UberShader::Index ubershader_index;
    ubershader_index.set(quality_info_, settings_.quality);
    context.materials[id()].get(mesh_instance().instance_parts[0].material.id).shader_program = 
        ubershader(context).get(ubershader_index);

    prepare_draw_call(draw_calls[0], context, scene_context, render_context, default_render_target());

    Material& material = context.materials[id()].get(second_pass_mesh_.instance_parts[0].material.id);

    ubershader_index.set(pass_info_, 1);
    material.shader_program = ubershader(context).get(ubershader_index);
    material.uniforms[0] = render_context.main_camera.percamera_uniform;

    clear_command_.reset();
    setup_draw_call(draw_calls[1], second_pass_mesh_.instance_parts[0], second_pass_mesh_.mesh.parts[0], output_rt_, &clear_command_);
}

bool DOFMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    if (!PosteffectMaterialSystemBase::init(context, material_system_context))
    {
        WARN_LOG("DOFMaterialSystem: PosteffectMaterialSystemBase::init failed");
        return false;
    }

    // parse options
    blur_resolve_pass_scale_ = material_system_context.options.get<float>("blur_pass_scale");
    if (blur_resolve_pass_scale_ == 0.0f) blur_resolve_pass_scale_ = 1.0f;

    UberShader& shader = ubershader(context);
    pass_info_ = shader.info("PASS");

    UniformBuffer& dof_uniform = create_and_get(context.uniform_pool);
    UniformBufferDesc desc;
    desc.size = sizeof(DOFShaderData);
    desc.unit = 1;
    desc.update_type = uniform_buffer_normal;
    if (!dof_uniform.init(desc))
    {
        ERROR_LOG("Can't initialize a UniformBuffer for DOF material");
        return false;
    }
    dof_uniform_ = dof_uniform.id();

    dof_shader_data_.planes = vec4(300.0f, 100.0f, 500.0f, 0.0f);
    dof_shader_data_.coc = vec4(4.0f, 0.0f, 0.0f, 0.0f);

    return true;
}

void DOFMaterialSystem::postinit(Context& context)
{
    MeshInstance& blur_resolve_pass_mesh_instance = mesh_instance();
    Material& blur_resolve_pass_material = context.materials[id()].get(blur_resolve_pass_mesh_instance.instance_parts[0].material.id);
    blur_resolve_pass_material.uniforms[1] = dof_uniform_;

    dof_pass_mesh_instance_ = blur_resolve_pass_mesh_instance;
    composite_pass_mesh_instance_ = blur_resolve_pass_mesh_instance;

    // R16 for blur. I could've used R16G16 format for the pair depth-blur, but I hope I'll add
    // a depth linearization pass as a part of the rendering pipeline
    RenderTargetDesc rt_desc;
    rt_desc.color_datatype[0] = format_float;
    rt_desc.color_format[0] = format_r16f;
    rt_desc.color_targets = 1;
    rt_desc.target = rt_readwrite;
    rt_desc.texture_type = texture_2d;
    rt_desc.use_depth = false;
    rt_desc.use_stencil = false;

    RenderTarget& blur_rt = context.render_target_manager.create(context, rt_desc, blur_resolve_pass_scale_);
    blur_rt_ = blur_rt.id();

    Material& dof_pass_material = create_and_get(context.materials[id()]);
    dof_pass_mesh_instance_.instance_parts[0].material.id = dof_pass_material.id;

    rt_desc.color_datatype[0] = format_ubyte;
    rt_desc.color_format[0] = format_rgba;
    RenderTarget& dof_rt = context.render_target_manager.create(context, rt_desc, 1.0f);
    dof_rt_ = dof_rt.id();

    Material& composite_material = create_and_get(context.materials[id()]);
    composite_pass_mesh_instance_.instance_parts[0].material.id = composite_material.id;

    UberShader& shader = ubershader(context);

    UberShader::Index index;
    index.set(pass_info_, 1);
    dof_pass_material.shader_program = shader.get(index);
    dof_pass_material.textures[input_texture_unit] = input(input_texture_unit);
    blur_rt.color_texture(dof_pass_material.textures[blur_texture_unit], 0);
    dof_pass_material.uniforms[1] = dof_uniform_;

    index.reset();
    index.set(pass_info_, 2);
    composite_material.shader_program = shader.get(index);
    composite_material.textures[input_texture_unit] = input(input_texture_unit);
    composite_material.textures[blur_texture_unit] = dof_pass_material.textures[blur_texture_unit];
    dof_rt.color_texture(composite_material.textures[dof_texture_unit], 0);
    composite_material.uniforms[1] = dof_uniform_;
}

void DOFMaterialSystem::destroy(Context& context)
{
    context.render_target_manager.destroy(dof_rt_, context);
    context.render_target_manager.destroy(blur_rt_, context);
    destroy_pool_object(context.uniform_pool, dof_uniform_);
    PosteffectMaterialSystemBase::destroy(context);
}

void DOFMaterialSystem::init_debug_views(Context& context)
{
    size_t debug_view_id = context.debug_views->add_view("Depth of field");
    DebugViews::DebugView& view = context.debug_views->get_view(debug_view_id);
    view.add("focus", 0.0f, 1000.0f, &dof_shader_data_.planes.x());
    view.add("near", 0.0f, 1000.0f, &dof_shader_data_.planes.y());
    view.add("far", 0.0f, 1000.0f, &dof_shader_data_.planes.z());
    view.add("max blur", 0.0f, 10.0f, &dof_shader_data_.planes.w());
}

void DOFMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    update_uniforms(context);

    DrawCall& dc1 = render_context.draw_calls.add();
    PosteffectMaterialSystemBase::prepare_draw_call(dc1, context, scene_context, render_context, blur_rt_);
    dc1.pass = 0;

    DrawCall& dc2 = render_context.draw_calls.add();
    setup_draw_call(dc2,
        dof_pass_mesh_instance_.instance_parts[0], dof_pass_mesh_instance_.mesh.parts[0], dof_rt_, &clear_command_simple_);
    dc2.pass = 1;

    DrawCall& dc3 = render_context.draw_calls.add();
    setup_draw_call(dc3,
        composite_pass_mesh_instance_.instance_parts[0], composite_pass_mesh_instance_.mesh.parts[0], default_render_target(), nullptr);
    dc3.pass = 2;
}

void DOFMaterialSystem::update_uniforms(Context& context)
{
    UniformBuffer& uniform = context.uniform_pool.get(dof_uniform_);
    uniform.update(dof_shader_data_);
}

bool SSAOMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    if (!PosteffectMaterialSystemBase::init(context, material_system_context))
    {
        ERROR_LOG("SSAO: can't initialize PosteffectMaterialSystemBase");
        return false;
    }

    UniformBuffer& uniform = create_and_get(context.uniform_pool);
    UniformBufferDesc desc;
    desc.size = sizeof(SSAOShaderData);
    desc.unit = 1;
    desc.update_type = uniform_buffer_normal;
    if (!uniform.init(desc))
    {
        ERROR_LOG("Can't initialize SSAO uniform");
        return false;
    }

    ssao_uniform_ = uniform.id();
    default_material(context).uniforms[1] = uniform.id();

    create_noise_texture(noise_texture_, context);

    ssao_shader_data_.ssaodata[0] = vec4(3.0f, 1.0f, 0.5f, 30.0f);
    ssao_shader_data_.ssaodata[1] = vec4(700.0f, 1000.0f, 1.0f, 8.0f);
    ssao_shader_data_.ssaodata[2] = vec4::zero();

    return true;
}

void SSAOMaterialSystem::destroy(Context& context)
{
    destroy_pool_object(context.texture_pool, noise_texture_.id);
    destroy_pool_object(context.uniform_pool, ssao_uniform_);
    PosteffectMaterialSystemBase::destroy(context);
}

void SSAOMaterialSystem::init_debug_views(Context& context)
{
    size_t debug_view_id = context.debug_views->add_view("SSAO");
    DebugViews::DebugView& view = context.debug_views->get_view(debug_view_id);
    view.add("radius", 0.0f, 50.0f, &ssao_shader_data_.ssaodata[0].x());
    view.add("power", 1.0f, 5.0f, &ssao_shader_data_.ssaodata[0].y());
    view.add("threshold", 0.0f, 20.0f, &ssao_shader_data_.ssaodata[0].z());
    view.add("max distance", 0.0f, 500.0f, &ssao_shader_data_.ssaodata[0].w());
    view.add("fade start", 100.0f, 2000.0f, &ssao_shader_data_.ssaodata[1].x());
    view.add("fade distance", 100.0f, 2000.0f, &ssao_shader_data_.ssaodata[1].y());
    view.add("intensity", 0.0f, 10.0f, &ssao_shader_data_.ssaodata[1].z());
    view.add("samples", 1.0f, 24.0f, &ssao_shader_data_.ssaodata[1].w());
    view.add("rotation", 0.0f, 10.0f, &settings_.rotation_speed);
}

void SSAOMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    ssao_shader_data_.ssaodata[2].x() += render_context.fdelta * settings_.rotation_speed;
    UniformBuffer& uniform = context.uniform_pool.get(ssao_uniform_);
    uniform.update(ssao_shader_data_);

    PosteffectMaterialSystemBase::update(context, scene_context, render_context);
    // TODO: it's better to move the noise texture to inputs
    default_material(context).textures[noise_texture_unit] = noise_texture_;
}

void SSAOMaterialSystem::create_noise_texture(TextureInstance& texture_instance, Context& context)
{
    TextureDesc desc;
    desc.address_mode_s = desc.address_mode_t = texture_clamp;
    desc.anisotropic_level = 0.0f;
    desc.datatype = format_float;
    desc.format = format_rg16f;
    desc.height = desc.width = 4;
    desc.mag_filter = texture_filter_linear;
    desc.min_filter = texture_filter_linear;
    desc.mips = 1;
    desc.type = texture_2d;

    vec2 data[4 * 4];
    for (size_t i = 0; i < 4 * 4; ++i)
    {
        vec2 p(random(-1.0f, 1.0f), random(-1.0f, 1.0f));
        data[i] = p;
    }

    Texture& texture = create_and_get(context.texture_pool);
    texture.init(desc, reinterpret_cast<uint8_t*>(&data), sizeof(data));
    texture_instance.id = texture.id();
}

AverageLuminanceMaterialSystem::ReductionCommand::ReductionCommand()
{
    set_stages(render_stage_after_submit | render_stage_before_submit);
    mhe::prepare_draw_call(compute_call_);
}

void AverageLuminanceMaterialSystem::ReductionCommand::set_parameters(ShaderStorageBufferHandleType* buffers,
    const ReductionContext& reduction_context, size_t threads_number)
{
    buffers_[0] = buffers[0];
    buffers_[1] = buffers[1];
    threads_number_ = threads_number;
    reduction_context_ = reduction_context;
}

bool AverageLuminanceMaterialSystem::ReductionCommand::execute_impl(Context& context, RenderStage current_stage)
{
    if (current_stage == render_stage_before_submit)
        context.driver.clear(true, false, false);
    else
    {
        GPU_PROFILE("cs_reduction");
        ShaderStorageBuffer* buffers[2] = {&context.shader_storage_buffer_pool.get(buffers_[0]),
            &context.shader_storage_buffer_pool.get(buffers_[1])};
        // texture->buffer conversion
        compute_call_.shader_program = &context.shader_pool.get(reduction_context_.texture_to_buffer_shader);
        compute_call_.images[0] = &context.texture_pool.get(reduction_context_.input.id);
        compute_call_.image_access[0] = access_readonly;
        compute_call_.buffers[1] = buffers[0];
        size_t threads_number = compute_call_.shader_program->variable_value<size_t>(string("THREADS_NUMBER"));
        size_t workgroups_number = reduction_context_.input_size / threads_number;
        compute_call_.workgroups_number.set(workgroups_number, workgroups_number, 1);

        context.driver.memory_barrier(memory_barrier_image_fetch);
        context.driver.execute(context, &compute_call_, 1);
        context.driver.memory_barrier(memory_barrier_storage_buffer);

        // actual reduction
        compute_call_.shader_program = &context.shader_pool.get(reduction_context_.reduction_shader);
        const size_t reduction_threads = threads_number_;
        size_t input_size = buffers[0]->size() / sizeof(float);
        size_t output_size = input_size;
        size_t input_buffer_index = 0;
        while (output_size > 1)
        {
            output_size /= reduction_threads;
            compute_call_.buffers[0] = buffers[input_buffer_index];
            input_buffer_index ^= 1;
            compute_call_.buffers[1] = buffers[input_buffer_index];
            compute_call_.workgroups_number.set(output_size, 1, 1);
            context.driver.execute(context, &compute_call_, 1);
            context.driver.memory_barrier(memory_barrier_storage_buffer);
        }
    }

    return true;
}

bool AverageLuminanceMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    if (!PosteffectMaterialSystemBase::init(context, material_system_context))
        return false;
    const mhe::string& reduction_shader_name = material_system_context.options.get<mhe::string>("reduction_shader");
    ASSERT(!reduction_shader_name.empty(), "AverageLuminanceMaterialSystem requires reduction shader name to be set");
    const mhe::string& texture_to_buffer_shader_name = material_system_context.options.get<string>("texture_to_buffer");
    ASSERT(!texture_to_buffer_shader_name.empty(), "AverageLuminanceMaterialSystem requires texture to buffer shader name to be set");
    size_t reduction_buffer_size = material_system_context.options.get<size_t>("output_size");
    ASSERT(reduction_buffer_size > 0, "Invalid reduction buffer size");
    const string& adaptation_shader_name = material_system_context.options.get<string>("adaptation_shader");
    ASSERT(!adaptation_shader_name.empty(), "AverageLuminanceMaterialSystem requires adaptation shader name to be set");

    Shader reduction_shader;
    bool res = context.shader_manager.get(reduction_shader, reduction_shader_name);
    ASSERT(res, "Can't get a reduction shader with name:" << reduction_shader_name);
    reduction_shader_ = context.ubershader_pool.get(reduction_shader.shader_program_handle).get_default();

    Shader texture_to_buffer_shader;
    res = context.shader_manager.get(texture_to_buffer_shader, texture_to_buffer_shader_name);
    ASSERT(res, "Can't get a texture to buffer shader with name:" << texture_to_buffer_shader_name);
    texture_to_buffer_shader_ = context.ubershader_pool.get(texture_to_buffer_shader.shader_program_handle).get_default();

    Shader adaptation_shader;
    res = context.shader_manager.get(adaptation_shader, adaptation_shader_name);
    ASSERT(res, "Can't get a texture to buffer shader with name:" << texture_to_buffer_shader_name);
    adaptation_shader_ = context.ubershader_pool.get(adaptation_shader.shader_program_handle).get_default();

    // calculate the index of the result buffer
    size_t tmp_size = reduction_buffer_size;
    size_t tmp_index = 0;
    while (tmp_size > 1)
    {
        tmp_index ^= 1;
        tmp_size /= reduction_threads_number;
    }

    UniformBuffer& adaptation_uniform = create_and_get(context.uniform_pool);
    UniformBufferDesc uniform_buffer_desc;
    uniform_buffer_desc.size = sizeof(AdaptationShaderData);
    uniform_buffer_desc.unit = 2;
    res = adaptation_uniform.init(uniform_buffer_desc);
    ASSERT(res, "Can't initialize a uniform for adaptation material");
    adaptation_uniform_ = adaptation_uniform.id();

    ShaderStorageBufferDesc shader_storage_buffer_desc;
    shader_storage_buffer_desc.format = format_float;
    shader_storage_buffer_desc.update_type = buffer_update_type_dynamic;
    shader_storage_buffer_desc.size = reduction_buffer_size * reduction_buffer_size * sizeof(float);
    for (size_t i = 0; i < 2; ++i)
    {
        ShaderStorageBuffer& shader_storage_buffer = create_and_get(context.shader_storage_buffer_pool);
        shader_storage_[i] = shader_storage_buffer.id();
        res = shader_storage_buffer.init(shader_storage_buffer_desc, nullptr, 0);
        ASSERT(res, "Can't init a shader storage buffer");
    }

    // setup buffers
    set_buffer(0, shader_storage_[tmp_index]);
    set_buffer(1, shader_storage_[tmp_index ^ 1]); // just in case

    // create two render targets. The first one is for gathering luminance and the second for holding adapted luminance
    RenderTargetDesc render_target_desc;
    render_target_desc.color_format[0] = format_r32f;
    render_target_desc.color_targets = 1;
    render_target_desc.height = reduction_buffer_size;
    render_target_desc.width = reduction_buffer_size;
    render_target_desc.texture_type = texture_2d;
    render_target_desc.use_depth = false;
    render_target_desc.use_stencil = false;
    render_target_desc.target = rt_readwrite;
    render_target_desc.color_datatype[0] = format_default;
    RenderTarget& render_target = create_and_get(context.render_target_pool);
    res = render_target.init(context, render_target_desc);
    ASSERT(res, "Can't initialize the output");

    for (size_t i = 0; i < 2; ++i)
    {
        RenderTarget& adaptation_render_target = create_and_get(context.render_target_pool);
        res = adaptation_render_target.init(context, render_target_desc);
        ASSERT(res, "Can't initialize the adaptation RT " << i);
        adaptation_render_target_[i] = adaptation_render_target.id();
    }
    adaptation_rt_index_ = 2; // we need to skip the first iteration

    Material& material = create_and_get(context.materials[id()]);
    material.shader_program = adaptation_shader_;
    material.uniforms[0] = adaptation_uniform_;
    render_target.color_texture(material.textures[0], 0);
    adaptation_material_ = material.id;
    
    render_target_ = render_target.id();
    RenderState& render_state = context.render_state_pool.get(mesh_instance().instance_parts[0].render_state_id);
    ViewportDesc viewport_desc;
    viewport_desc.viewport.set(0, 0, reduction_buffer_size, reduction_buffer_size);
    render_state.update_viewport(viewport_desc);

    ReductionContext reduction_context;
    reduction_context.input_size = reduction_buffer_size;
    reduction_context.reduction_shader = reduction_shader_;
    reduction_context.texture_to_buffer_shader = texture_to_buffer_shader_;
    reduction_command_.set_parameters(shader_storage_, reduction_context, reduction_threads_number);

    return true;
}

void AverageLuminanceMaterialSystem::destroy(Context& context)
{
    destroy_pool_object(context.shader_storage_buffer_pool, shader_storage_[0]);
    destroy_pool_object(context.shader_storage_buffer_pool, shader_storage_[1]);
    destroy_pool_object(context.uniform_pool, adaptation_uniform_);
    destroy_pool_object(context.render_target_pool, render_target_);
    destroy_pool_object(context.render_target_pool, adaptation_render_target_[0]);
    destroy_pool_object(context.render_target_pool, adaptation_render_target_[1]);
    PosteffectMaterialSystemBase::destroy(context);
}

void AverageLuminanceMaterialSystem::init_debug_views(Context& context)
{
    settings_.adaptation_rate = 6.0f;

    size_t debug_view_id = context.debug_views->add_view("Adaptation");
    DebugViews::DebugView& view = context.debug_views->get_view(debug_view_id);
    view.add(string("rate"), 0.1f, 10.0f, &settings_.adaptation_rate);
    view.add(string("fixed lum"), 0.0f, 2.0f, &settings_.fixed_luminance_value);
}

void AverageLuminanceMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    if (settings_.mode == adaptation_mode)
    {
        AdaptationShaderData shader_data;
        shader_data.data.set(settings_.adaptation_rate, render_context.fdelta, 0.0f, 0.0f);
        UniformBuffer& uniform_buffer = context.uniform_pool.get(adaptation_uniform_);
        uniform_buffer.update(shader_data);

        // The first draw call is to calculate per-pixel luminance
        DrawCall& draw_call = render_context.draw_calls.add();
        prepare_draw_call(draw_call, context, scene_context, render_context, render_target_);
        draw_call.pass = 0;
        draw_call.command = &clear_command_;
        // And the second is to perform luminance adaptation, also per-pixel
        {
            DrawCall& dc = render_context.draw_calls.add();
            dc = draw_call;
            dc.material.id = adaptation_material_;
            dc.pass = 1;
            dc.command = &reduction_command_;

            uint8_t adapted_luminance_index = adaptation_rt_index_;
            uint8_t prev_luminance_index = adaptation_rt_index_ ^ 1;
            Material& material = context.materials[id()].get(adaptation_material_);
            if (adaptation_rt_index_ == 2)
            {
                material.textures[1] = material.textures[0];
                adaptation_rt_index_ = 0;
                adapted_luminance_index = 0;
            }
            else
            {
                RenderTarget& rt = context.render_target_pool.get(adaptation_render_target_[prev_luminance_index]);
                rt.color_texture(material.textures[1], 0);
            }
            dc.render_target = adaptation_render_target_[adapted_luminance_index];

            TextureInstance output_texture;
            context.render_target_pool.get(dc.render_target).color_texture(output_texture, 0);
            reduction_command_.set_input(output_texture);

            adaptation_rt_index_ ^= 1;
        }
    }
    else if (settings_.mode == fixed_luminance_mode)
    {
        ShaderStorageBuffer& shader_storage_buffer = context.shader_storage_buffer_pool.get(buffer(0));
        shader_storage_buffer.update(reinterpret_cast<const uint8_t*>(&settings_.fixed_luminance_value), sizeof(float));
    }
    else
    {
        ASSERT(0, "Invalid Adaptation mode");
    }
}

bool BloomMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    if (!PosteffectMaterialSystemBase::init(context, material_system_context))
    {
        ERROR_LOG("Can't initialize BloomMaterialSystem");
        return false;
    }

    UniformBuffer& uniform_buffer = create_and_get(context.uniform_pool);

    UniformBufferDesc uniform_buffer_desc;
    uniform_buffer_desc.size = sizeof(ShaderData);
    uniform_buffer_desc.unit = 1;
    uniform_buffer_desc.update_type = uniform_buffer_normal;

    if (!uniform_buffer.init(uniform_buffer_desc))
    {
        ERROR_LOG("Can't initialize a UniformBuffer for BloomMaterialSystem");
        destroy(context);
        return false;
    }

    Material& original_material = default_material(context);

    original_material.uniforms[1] = uniform_buffer.id();
    uniform_ = uniform_buffer.id();
    set_uniform(1, uniform_);

    Shader shader;
    context.shader_manager.get(shader, material_system_context.options.get<string>("copy_shader"));
    ASSERT(is_handle_valid(shader.shader_program_handle), "Can't load copy shader");
    ShaderProgramHandleType copy_shader_program = context.ubershader_pool.get(shader.shader_program_handle).get_default();

    for (size_t i = 0; i < steps_number; ++i)
    {
        RenderState& render_state = create_and_get(context.render_state_pool);
        RenderStateDesc render_state_desc;
        render_state_desc.blend.enabled = false;
        render_state_desc.depth.test_enabled = false;
        render_state_desc.depth.write_enabled = false;
        render_state_desc.rasterizer.cull = cull_none;
        render_state_desc.scissor.enabled = false;
        if (!render_state.init(render_state_desc))
        {
            ERROR_LOG("Can't init a RenderState for bloom step " << i);
        }
        render_states_[i] = render_state.id();

        // and init a new material
        Material& material = create_and_get(context.materials[id()]);
        material.uniforms[0] = original_material.uniforms[0];
        material.uniforms[1] = original_material.uniforms[1];
        material.shader_program = copy_shader_program;
        materials_[i] = material.id;
    }

    // init blur material system
    blur_material_system_ = new BlurMaterialSystem;
    MaterialSystemContext blur_material_system_context;
    blur_material_system_context.instance_name = "bloom_blur";
    blur_material_system_context.priority = material_system_context.priority;
    blur_material_system_context.material_instances_number = 2;
    blur_material_system_context.shader_name = material_system_context.options.get<string>("blur_shader");
    context.material_systems.add(blur_material_system_);
    context.materials[blur_material_system_->id()].resize(2);
    if (!blur_material_system_->init(context, blur_material_system_context))
    {
        ERROR_LOG("Can't initialize a BlurMaterialSystem for bloom");
        destroy(context);
        return false;
    }
    blur_material_system_->PosteffectMaterialSystemBase::create_output(context, 0, 0.25f, format_rgba16f);

    return true;
}

void BloomMaterialSystem::destroy(Context& context)
{
    destroy_pool_object(context.uniform_pool, uniform_);
    for (size_t i = 0; i < steps_number; ++i)
        destroy_pool_object(context.render_state_pool, render_states_[i]);
    PosteffectMaterialSystemBase::destroy(context);
}

void BloomMaterialSystem::init_debug_views(Context& context)
{
    size_t debug_view_id = context.debug_views->add_view("Bloom");
    DebugViews::DebugView& view = context.debug_views->get_view(debug_view_id);
    settings_.exposure_key = 0.4f;
    settings_.threshold = 3.0f;
    settings_.intensity = 0.7f;
    view.add(string("exposure key"), 0.0f, 5.0f, &settings_.exposure_key);
    view.add(string("threshold"), 0.0f, 5.0f, &settings_.threshold);
    view.add(string("intensity"), 0.0f, 5.0f, &settings_.intensity);
}

void BloomMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    UniformBuffer& uniform_buffer = context.uniform_pool.get(uniform_);
    ShaderData shader_data;
    shader_data.settings.set_x(settings_.exposure_key);
    shader_data.settings.set_y(settings_.threshold);
    shader_data.settings.set_z(settings_.intensity);
    uniform_buffer.update(shader_data);

    RenderTargetHandleType original_render_target_id = default_render_target();
    ASSERT(original_render_target_id != mhe::default_render_target, "Bloom must use a separate render target for the first pass");
    RenderTarget& original_render_target = context.render_target_pool.get(original_render_target_id);
    // 1) find the brightest pixels
    DrawCall& draw_call = render_context.draw_calls.add();
    PosteffectMaterialSystemBase::prepare_draw_call(draw_call, context, scene_context, render_context, original_render_target_id);
    draw_call.pass = 0;
    // 2) downsample to the 1/2
    RenderTarget& rt_1_2 = context.render_target_manager.get_temp_render_target(context, format_rgba16f, 0.5f);
    {
        DrawCall& dc = render_context.draw_calls.add();
        dc = draw_call;
        copy(context, dc, rt_1_2, original_render_target, 0);
    }
    // 3) downsample to the 1/4
    RenderTarget& rt_1_4 = context.render_target_manager.get_temp_render_target(context, format_rgba16f, 0.25f);
    {
        DrawCall& dc = render_context.draw_calls.add();
        dc = draw_call;
        copy(context, dc, rt_1_4, rt_1_2, 1);
    }
    // 4) blur
    {
        DrawCall& dc = render_context.draw_calls.add();
        DrawCall& dc2 = render_context.draw_calls.add();
        blur_material_system_->set_priority(priority());
        TextureInstance blur_input;
        rt_1_4.color_texture(blur_input, 0);
        blur_material_system_->set_input(0, blur_input);
        blur_material_system_->setup_draw_calls(context, scene_context, &dc, 2, render_context);
        dc.pass = 3;
        dc2.pass = 4;
    }
    // 5) upsample to 1/2
    {
        DrawCall& dc = render_context.draw_calls.add();
        dc = draw_call;
        copy(context, dc, rt_1_2, context.render_target_pool.get(blur_material_system_->render_target_id()), 4);
    }
    // 6) upsample to the original size
    {
        DrawCall& dc = render_context.draw_calls.add();
        dc = draw_call;
        copy(context, dc, original_render_target, rt_1_2, 5);
    }
}

void BloomMaterialSystem::copy(Context& context, DrawCall& draw_call, RenderTarget& dst, const RenderTarget& from, size_t pass)
{
    draw_call.render_target = dst.id();
    RenderState& render_state = context.render_state_pool.get(render_states_[pass]);
    ViewportDesc viewport_desc;
    viewport_desc.viewport.set(0, 0, dst.width(), dst.height());
    render_state.update_viewport(viewport_desc);
    draw_call.render_state = render_states_[pass];
    draw_call.pass = pass + 1;
    draw_call.command = &clear_command_simple_;
    Material& material = context.materials[id()].get(materials_[pass]);
    draw_call.material.id = materials_[pass];
    material.textures[0].id = Texture::invalid_id;
    from.color_texture(material.textures[0], 0);
    ASSERT(material.textures[0].id != Texture::invalid_id, "Invalid source id");
}

}
