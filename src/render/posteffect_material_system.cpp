#include "render/posteffect_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/instances.hpp"
#include "render/utils/simple_meshes.hpp"
#include "render/render_context.hpp"
#include "utils/strutils.hpp"
#include "debug/debug_views.hpp"

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

    clear_command_.set_driver(&context.driver);

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
    DrawCallData& draw_call_data = create_and_get(context.draw_call_data_pool);
    RenderState& render_state = create_and_get(context.render_state_pool);

    draw_call_data.state = render_state.id();
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

    mesh_instance.mesh.parts.resize(1);
    mesh_instance.instance_parts.resize(1);
    if (!utils::create_fullscreen_quad(mesh_instance, context))
    {
        ERROR_LOG("Can't create fullscreen quad");
        return false;
    }
    mesh_instance.mesh.parts[0].render_data.layout = layout();

    mesh_instance.instance_parts[0].draw_call_data = draw_call_data.id;

    Material& material = create_and_get(context.materials[id()]);
    material.shader_program = default_program(context).id();
    mesh_instance.instance_parts[0].material.material_system = id();
    mesh_instance.instance_parts[0].material.id = material.id;

    return true;
}

void PosteffectDebugMaterialSystem::close()
{
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
        setup_draw_call(render_context.draw_calls.add(), fullscreen_mesh_.instance_parts[0], fullscreen_mesh_.mesh.parts[0], &clear_command_);
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

            setup_draw_call(render_context.draw_calls.add(), mesh_[j].instance_parts[0], mesh_[j].mesh.parts[0], &clear_command_);
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
    inputs_number_(0), outputs_number_(0), profile_command_(name), framebuffer_input_(invalid_index)
{}

bool PosteffectMaterialSystemBase::init(Context& context, const MaterialSystemContext& material_system_context)
{
    set_layout(FullscreenLayout::handle);

    if (!context.shader_manager.get(shader(), material_system_context.shader_name))
    {
        ERROR_LOG("Can't get shader:" << material_system_context.shader_name);
        return false;
    }

    clear_command_.set_driver(&context.driver);

    inputs_number_shader_info_ = ubershader(context).info("INPUTS");

    return init_mesh(context, material_system_context);
}

void PosteffectMaterialSystemBase::close()
{}

void PosteffectMaterialSystemBase::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void PosteffectMaterialSystemBase::prepare_draw_call(DrawCall& draw_call, Context& context, SceneContext& /*scene_context*/, RenderContext& render_context)
{
    list_of_commands_.clear();
    clear_command_.reset();
    if (framebuffer_input_ != invalid_index) // current framebuffer as input
    {
        copy_framebuffer_command_.set_texture(&context.texture_pool.get(inputs_[framebuffer_input_].id));
        list_of_commands_.add_command(&copy_framebuffer_command_);
    }

    DrawCallData& draw_call_data = context.draw_call_data_pool.get(mesh_.instance_parts[0].draw_call_data);
    if (draw_call_data.render_target != default_render_target &&
        draw_call_data.render_target != RenderTarget::invalid_id)
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
    material.shader_program = shader.get(ubershader_index);
    setup_draw_call(draw_call, mesh_.instance_parts[0], mesh_.mesh.parts[0], command);
}

void PosteffectMaterialSystemBase::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    prepare_draw_call(render_context.draw_calls.add(), context, scene_context, render_context);
}

bool PosteffectMaterialSystemBase::init_mesh(Context& context, const MaterialSystemContext& material_system_context)
{
    DrawCallData& draw_call_data = create_and_get(context.draw_call_data_pool);
    RenderState& render_state = create_and_get(context.render_state_pool);

    bool use_blend = material_system_context.options.get<bool>(string("blend"));

    draw_call_data.state = render_state.id();
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
        create_output(draw_call_data, context, 0, types_cast<float>(outputs_scale[0]), format_rgba);
    }

    mesh_.mesh.parts.resize(1);
    mesh_.instance_parts.resize(1);
    if (!utils::create_fullscreen_quad(mesh_, context))
    {
        ERROR_LOG("Can't create fullscreen quad");
        return false;
    }
    mesh_.mesh.parts[0].render_data.layout = layout();

    mesh_.instance_parts[0].draw_call_data = draw_call_data.id;

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
    DrawCallData& draw_call_data = context.draw_call_data_pool.get(mesh_.instance_parts[0].draw_call_data);
    ASSERT(draw_call_data.render_target == default_render_target || draw_call_data.render_target == RenderTarget::invalid_id,
        "RenderTarget has been set already");
    return create_output(draw_call_data, context, index, scale, format);
}

void PosteffectMaterialSystemBase::fill_render_target_desc(RenderTargetDesc& desc, int format) const
{
    desc.target = rt_readwrite;
    desc.use_depth = false;
    desc.use_stencil = false;
    desc.color_targets = 1;
    desc.color_datatype[0] = format_default;
    desc.color_format[0] = format != format_max ? format : format_rgba;
}

bool PosteffectMaterialSystemBase::create_output(DrawCallData& draw_call_data, Context& context, size_t index, float scale, int format)
{
    RenderTargetDesc render_target_desc;
    fill_render_target_desc(render_target_desc, format);
    RenderTarget& render_target = context.render_target_manager.create(context, render_target_desc, scale);
    draw_call_data.render_target = render_target.id();
    const TextureInstance* ids = nullptr;
    render_target.color_textures(&ids);
    set_output(index, ids[0]);

    if (scale != 1.0f)
    {
        RenderState& render_state = context.render_state_pool.get(draw_call_data.state);
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
    DrawCallData& first_pass_draw_call_data = context.draw_call_data_pool.get(first_pass_mesh.instance_parts[0].draw_call_data);
    second_pass_mesh_ = first_pass_mesh;
    DrawCallData& second_pass_draw_call_data = create_and_get(context.draw_call_data_pool);
    second_pass_draw_call_data = first_pass_draw_call_data;
    second_pass_mesh_.instance_parts[0].draw_call_data = second_pass_draw_call_data.id;

    Material& material = create_and_get(context.materials[id()]);
    second_pass_mesh_.instance_parts[0].material.id = material.id;
    material.uniforms[1] = uniform_;

    clear_command_.set_driver(&context.driver);

    return true;
}

bool BlurMaterialSystem::create_output(DrawCallData& draw_call_data, Context& context, size_t index, float scale, int format)
{
    if (!PosteffectMaterialSystemBase::create_output(draw_call_data, context, index, scale, format))
        return false;

    DrawCallData& second_pass_draw_call_data = context.draw_call_data_pool.get(second_pass_mesh_.instance_parts[0].draw_call_data);

    RenderTargetDesc render_target_desc;
    fill_render_target_desc(render_target_desc);
    RenderTarget& render_target = context.render_target_manager.create(context, render_target_desc, scale);
    second_pass_draw_call_data.render_target = render_target.id();

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
    ShaderData data;
    data.params.set_x(settings_.size);
    context.uniform_pool.get(uniform_).update(data);

    UberShader::Index ubershader_index;
    ubershader_index.set(quality_info_, settings_.quality);
    context.materials[id()].get(mesh_instance().instance_parts[0].material.id).shader_program = 
        ubershader(context).get(ubershader_index);

    PosteffectMaterialSystemBase::update(context, scene_context, render_context);

    Material& material = context.materials[id()].get(second_pass_mesh_.instance_parts[0].material.id);
    
    ubershader_index.set(pass_info_, 1);
    material.shader_program = ubershader(context).get(ubershader_index);
    material.uniforms[0] = render_context.main_camera.percamera_uniform;

    clear_command_.reset();
    setup_draw_call(render_context.draw_calls.add(), second_pass_mesh_.instance_parts[0], second_pass_mesh_.mesh.parts[0], &clear_command_);
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

    clear_command_simple_.set_driver(&context.driver);

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

    dof_shader_data_.planes = vec4(300.0f, 100.0f, 500.0f, 1.0f);
    dof_shader_data_.coc = vec4(4.0f, 0.0f, 0.0f, 0.0f);

    return true;
}

void DOFMaterialSystem::postinit(Context& context)
{
    MeshInstance& blur_resolve_pass_mesh_instance = mesh_instance();
    Material& blur_resolve_pass_material = context.materials[id()].get(blur_resolve_pass_mesh_instance.instance_parts[0].material.id);
    DrawCallData& blur_resolve_pass_draw_call_data = context.draw_call_data_pool.get(blur_resolve_pass_mesh_instance.instance_parts[0].draw_call_data);
    RenderTarget::IdType composite_pass_rt_id = blur_resolve_pass_draw_call_data.render_target;
    blur_resolve_pass_material.uniforms[1] = dof_uniform_;

    dof_pass_mesh_instance_ = blur_resolve_pass_mesh_instance;
    DrawCallData& dof_pass_pass_draw_call_data = create_and_get(context.draw_call_data_pool);
    dof_pass_pass_draw_call_data = blur_resolve_pass_draw_call_data;
    dof_pass_mesh_instance_.instance_parts[0].draw_call_data = dof_pass_pass_draw_call_data.id;

    composite_pass_mesh_instance_ = blur_resolve_pass_mesh_instance;
    DrawCallData& composite_pass_draw_call_data = create_and_get(context.draw_call_data_pool);
    composite_pass_draw_call_data = blur_resolve_pass_draw_call_data;
    composite_pass_mesh_instance_.instance_parts[0].draw_call_data = composite_pass_draw_call_data.id;

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
    blur_resolve_pass_draw_call_data.render_target = blur_rt.id();

    Material& dof_pass_material = create_and_get(context.materials[id()]);
    dof_pass_mesh_instance_.instance_parts[0].material.id = dof_pass_material.id;

    rt_desc.color_datatype[0] = format_ubyte;
    rt_desc.color_format[0] = format_rgba;
    RenderTarget& dof_rt = context.render_target_manager.create(context, rt_desc, 1.0f);
    dof_pass_pass_draw_call_data.render_target = dof_rt.id();

    Material& composite_material = create_and_get(context.materials[id()]);
    composite_pass_mesh_instance_.instance_parts[0].material.id = composite_material.id;
    composite_pass_draw_call_data.render_target = composite_pass_rt_id;

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
    PosteffectMaterialSystemBase::prepare_draw_call(dc1, context, scene_context, render_context);
    dc1.pass = 0;

    DrawCall& dc2 = render_context.draw_calls.add();
    setup_draw_call(dc2,
        dof_pass_mesh_instance_.instance_parts[0], dof_pass_mesh_instance_.mesh.parts[0], &clear_command_simple_);
    dc2.pass = 1;

    DrawCall& dc3 = render_context.draw_calls.add();
    setup_draw_call(dc3,
        composite_pass_mesh_instance_.instance_parts[0], composite_pass_mesh_instance_.mesh.parts[0], nullptr);
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
    ssao_shader_data_.ssaodata[1] = vec4(700.0f, 1000.0f, 1.0f, 0.0f);

    return true;
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
}

void SSAOMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
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

}
