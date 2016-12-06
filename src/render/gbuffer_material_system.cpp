#include "render/gbuffer_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/uniforms.hpp"
#include "render/render_context.hpp"
#include "render/instances.hpp"
#include "render/light_instance_methods.hpp"
#include "render/utils/simple_meshes.hpp"
#include "render/render_globals.hpp"
#include "render/renderer.hpp"
#include "debug/debug_views.hpp"

namespace mhe {

GBufferFillMaterialSystem::GBufferFillMaterialSystem() :
    profile_command_("gbuffer_fill"),
    use_baked_lighting_(true),
    use_normalmapping_(global::use_normalmaps.value())
{
    gbuffer_desc_.target = rt_readwrite;
    gbuffer_desc_.width = gbuffer_desc_.height = 0;
    gbuffer_desc_.color_datatype[0] = format_ubyte;
    gbuffer_desc_.color_datatype[1] = format_float;
    gbuffer_desc_.color_datatype[2] = format_float;
    gbuffer_desc_.color_format[0] = format_rgba;
    gbuffer_desc_.color_format[1] = format_rgba16f;
    gbuffer_desc_.color_format[2] = format_rgba16f;
    gbuffer_desc_.color_targets = 3;
    gbuffer_desc_.depth_format = format_d24f;
    gbuffer_desc_.depth_datatype = format_float;
    gbuffer_desc_.use_depth = true;
    gbuffer_desc_.use_stencil = false;
}

bool GBufferFillMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    set_layout(StandartGeometryLayout::handle);

    profile_command_.set_stages(render_stage_begin_priority | render_stage_end_priority);
    list_of_commands_.add_command(&clear_command_);
    list_of_commands_.add_command(&profile_command_);

    if (!context.shader_manager.get(shader(), material_system_context.shader_name))
        return false;

    render_target_ = context.render_target_pool.create();
    RenderTarget& render_target = context.render_target_pool.get(render_target_);
    if (!gbuffer_desc_.width) gbuffer_desc_.width = context.window_system.width();
    if (!gbuffer_desc_.height) gbuffer_desc_.height = context.window_system.height();
    if (!render_target.init(context, gbuffer_desc_))
    {
        destroy(context);
        return false;
    }
    return true;
}

void GBufferFillMaterialSystem::destroy(Context& context)
{
    destroy_pool_object(context.render_target_pool, render_target_);
}

void GBufferFillMaterialSystem::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    standart_material_setup(context, scene_context, instance_parts, parts, model_contexts, count);

    UberShader& shader = ubershader(context);
    const UberShader::Info& normalmap_info = shader.info("NORMALMAP");
    const UberShader::Info& skinning_info = shader.info("SKINNING");

    for (size_t i = 0; i < count; ++i)
    {
        Material& material = context.materials[id()].get(instance_parts[i].material.id);
        size_t use_normalmap = material.textures[normal_texture_unit].id != Texture::invalid_id ? 1 : 0;
        if (!global::use_normalmaps.value())
            use_normalmap = 0;
        size_t use_skinning = parts[i].render_data.layout == SkinnedGeometryLayout::handle ? 1 : 0;

        if (use_skinning)
            material.texture_buffers[animation_texture_unit] = model_contexts[i].animation_texture_buffer;

        material.texture_buffers[baked_light_texture_unit] = model_contexts[i].baked_light_texture_buffer;

        UberShader::Index index;
        index.set(normalmap_info, use_normalmap);
        index.set(skinning_info, use_skinning);
        material.shader_program = shader.get(index);

        MaterialData& material_data = context.material_data_pool.get(instance_parts[i].material_id);
        UniformBuffer::IdType material_uniform_id = create_material_uniform(context, material_data);

        if (is_handle_valid(material_uniform_id))
            material.uniforms[material_data_unit] = material_uniform_id;
    }
}

void GBufferFillMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    MATERIAL_UPDATE_WITH_COMMAND(context, scene_context, render_context, update, render_target_, &list_of_commands_);
}

void GBufferFillMaterialSystem::update(Context& context, SceneContext& /*scene_context*/, RenderContext& render_context, MeshPartInstance* instance_parts, MeshPart* parts, size_t count)
{
    UberShader& shader = ubershader(context);
    const UberShader::Info& normalmap_info = shader.info("NORMALMAP");
    const UberShader::Info& roughness_info = shader.info("ROUGHNESS_MAP");
    const UberShader::Info& metalness_info = shader.info("METALNESS_MAP");
    const UberShader::Info& skinning_info = shader.info("SKINNING");
    const UberShader::Info& baked_light_info = shader.info("BAKED_LIGHT");
    UberShader::Index index;
    clear_command_.reset();
    for (size_t i = 0; i < count; ++i)
    {
        Material& material = context.materials[id()].get(instance_parts[i].material.id);
        material.uniforms[perframe_data_unit] = render_context.main_camera.percamera_uniform;
        const size_t use_normalmap = material.textures[normal_texture_unit].id != Texture::invalid_id && use_normalmapping_ ? 1 : 0;
        const size_t use_roughness_map = is_handle_valid(material.textures[roughness_texture_unit].id) ? 1 : 0;
        const size_t use_metalness_map = is_handle_valid(material.textures[metalness_texture_unit].id) ? 1 : 0;
        const size_t use_skinning = material.texture_buffers[animation_texture_unit] != TextureBuffer::invalid_id ? 1 : 0;
        const size_t use_baked_light = use_baked_lighting_ && material.texture_buffers[baked_light_texture_unit] != TextureBuffer::invalid_id ? 1 : 0;
        index.set(normalmap_info, use_normalmap);
        index.set(roughness_info, use_roughness_map);
        index.set(metalness_info, use_metalness_map);
        index.set(skinning_info, use_skinning);
        index.set(baked_light_info, use_baked_light);
        material.shader_program = shader.get(index);

#ifdef MHE_UPDATE_MATERIAL
        if (is_handle_valid(material.uniforms[material_data_unit]))
        {
            const MaterialData& material_data = context.material_data_pool.get(instance_parts[i].material_id);
            update_material_data(context.uniform_pool.get(material.uniforms[material_data_unit]), material_data);
        }
#endif
    }
}

void GBufferFillMaterialSystem::setup_uniforms(Material& material, Context& /*context*/, SceneContext& /*scene_context*/, const MeshPartInstance& /*part*/, const ModelContext& model_context)
{
    material.uniforms[0] = transform_uniform_;
    material.uniforms[1] = model_context.transform_uniform;
}

void GBufferFillMaterialSystem::output(Context& context, size_t unit, TextureInstance& texture) const
{
    RenderTarget& render_target = context.render_target_pool.get(render_target_);
    if (unit == gbuffer_depth_render_target_index)
        render_target.depth_texture(texture);
    else
        render_target.color_texture(texture, unit);
}

UniformBufferHandleType GBufferFillMaterialSystem::create_material_uniform(Context& context, const MaterialData& material_data)
{
    UniformBufferHandleType uniform_id = UniformBuffer::invalid_id;
    size_t uniform_data_size = 0;
    switch (material_data.lighting_model)
    {
    case material_type_blinn_phong:
        uniform_data_size = sizeof(PhongMaterialData);
        uniform_id = context.uniform_pool.create();
        break;
    case material_type_ggx:
        uniform_data_size = sizeof(PBRMaterialData);
        uniform_id = context.uniform_pool.create();
        break;
    default:
        return UniformBuffer::invalid_id;
    }

    UniformBuffer& uniform_buffer = context.uniform_pool.get(uniform_id);
    UniformBufferDesc uniform_buffer_desc;
    uniform_buffer_desc.unit = material_data_unit;
    uniform_buffer_desc.update_type = uniform_buffer_static;
    uniform_buffer_desc.size = uniform_data_size;

    bool res = uniform_buffer.init(uniform_buffer_desc);
    ASSERT(res, "Can't initialize material uniform");
    update_material_data(uniform_buffer, material_data);

    return uniform_id;
}

void GBufferFillMaterialSystem::update_material_data(UniformBuffer& uniform_buffer, const MaterialData& material_data)
{
    switch (material_data.lighting_model)
    {
    case material_type_blinn_phong:
        {
            PhongMaterialData shader_material_data;
            shader_material_data.diffuse = vec4(material_data.render_data.diffuse, 1.0f);
            shader_material_data.specular = vec4(material_data.render_data.specular, material_data.render_data.specular_shininess);
            shader_material_data.params = vec4(material_data.render_data.glossiness, 0.0f, 0.0f, 0.0f);
            uniform_buffer.update(shader_material_data);
        }
        break;

    case material_type_ggx:
        {
            PBRMaterialData shader_material_data;
            shader_material_data.parameters.set(material_data.render_data.roughness, material_data.render_data.metalness, 0.0f, 0.0f);
            uniform_buffer.update(shader_material_data);
        }
        break;
    default: break;
    }
}

void GBufferFillMaterialSystem::init_debug_views(Context& context)
{
    RenderTarget& render_target = context.render_target_pool.get(render_target_);
    TextureInstance texture_instance;
    render_target.color_texture(texture_instance, 0);
    context.debug_views->add_debug_buffer("albedo", texture_instance, Renderer::renderer_debug_mode_rgb);
    context.debug_views->add_debug_buffer("roughness", texture_instance, Renderer::renderer_debug_mode_a);
    render_target.color_texture(texture_instance, 1);
    context.debug_views->add_debug_buffer("normal", texture_instance, Renderer::renderer_debug_mode_rgb);
    context.debug_views->add_debug_buffer("metalness", texture_instance, Renderer::renderer_debug_mode_a);
    render_target.depth_texture(texture_instance);
    context.debug_views->add_debug_buffer("depth", texture_instance, Renderer::renderer_debug_mode_depth);
}

bool GBufferDrawMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    RenderTargetDesc light_buffer_desc;
    light_buffer_desc.target = rt_readwrite;
    light_buffer_desc.color_datatype[0] = format_float;
    light_buffer_desc.color_format[0] = format_rgba16f;
    light_buffer_desc.color_targets = 1;
    light_buffer_desc.use_depth = false;
    light_buffer_desc.use_stencil = false;
    light_buffer_desc.width = context.window_system.width();
    light_buffer_desc.height = context.window_system.height();
    light_buffer_render_target_ = context.render_target_pool.create();
    RenderTarget& light_render_target = context.render_target_pool.get(light_buffer_render_target_);
    if (!light_render_target.init(context, light_buffer_desc))
    {
        destroy(context);
        return false;
    }

    const TextureInstance* light_texture;
    light_render_target.color_textures(&light_texture);
    light_buffer_texture_ = light_texture[0];

    // TODO: need to get such settings from the shader
    if (!context.shader_manager.get(shader(), material_system_context.shader_name))
        return false;

    UberShader::Index index;
    const UberShader::Info& info = ubershader(context).info("LIGHT_TYPE");
    index.set(info, directional);

    profile_command_.set_stages(render_stage_begin_priority | render_stage_end_priority);
    list_of_commands_.add_command(&clear_command_);
    list_of_commands_.add_command(&profile_command_);

    if (!init_meshes(context))
    {
        destroy(context);
        return false;
    }
    return true;
}

bool GBufferDrawMaterialSystem::init_meshes(Context &context)
{
    render_state_ = context.render_state_pool.create();
    RenderState& render_state = context.render_state_pool.get(render_state_);
    RenderStateDesc desc;
    desc.depth.test_enabled = false;
    desc.depth.write_enabled = false;
    desc.blend.enabled = true;
    desc.blend.dstmode = blend_one;
    desc.blend.srcmode = blend_one;

    desc.rasterizer.cull = cull_none;
    if (!render_state.init(desc)) return false;

    if (!init_fullscreen_quad(context))
        return false;
    if (!init_sphere(context))
        return false;
    return init_conus(context);
}

bool GBufferDrawMaterialSystem::init_fullscreen_quad(Context& context)
{
    quad_mesh_.mesh.parts.resize(1);
    quad_mesh_.instance_parts.resize(1);

    if (!utils::create_fullscreen_quad(quad_mesh_, context))
    {
        ERROR_LOG("Can't create fullscreen quad");
        return false;
    }

    quad_mesh_.mesh.parts[0].render_data.layout = FullscreenLayout::handle;
    quad_mesh_.instance_parts[0].render_state_id = render_state_;
    return true;
}

bool GBufferDrawMaterialSystem::init_sphere(Context& context)
{
    sphere_mesh_.mesh.parts.resize(1);
    sphere_mesh_.instance_parts.resize(1);
    if (!utils::create_sphere(sphere_mesh_, context, 2))
    {
        ERROR_LOG("Can't create a sphere");
        return false;
    }

    sphere_mesh_.instance_parts[0].render_state_id = render_state_;

    return true;
}

bool GBufferDrawMaterialSystem::init_conus(Context& context)
{
    conus_mesh_.mesh.parts.resize(1);
    conus_mesh_.instance_parts.resize(1);
    if (!utils::create_conus(conus_mesh_, context, 1.0f, 1.0f, 8))
    {
        ERROR_LOG("Can't create a conus");
        return false;
    }

    conus_mesh_.instance_parts[0].render_state_id = render_state_;

    return true;
}

void GBufferDrawMaterialSystem::set_render_target(Context& context, RenderTarget::IdType render_target_id)
{
    render_target_ = render_target_id;
    RenderTarget& render_target = context.render_target_pool.get(render_target_);
    const TextureInstance* textures = nullptr;
    size_t number = render_target.color_textures(&textures);
    ASSERT(number != 0, "Invalid render target");
    albedo_texture_ = textures[0];
    normal_texture_ = textures[1];
    accumnulator_texture_ = textures[2];
    TextureInstance depth_texture;
    number = render_target.depth_texture(depth_texture);
    ASSERT(number != 0, "Invalid depth texture");
    depth_texture_ = depth_texture;
}

void GBufferDrawMaterialSystem::destroy(Context& context)
{
    destroy_mesh_instance(quad_mesh_, context);
    destroy_mesh_instance(sphere_mesh_, context);
    destroy_mesh_instance(conus_mesh_, context);

    destroy_pool_object(context.render_state_pool, render_state_);
}

void GBufferDrawMaterialSystem::init_debug_views(Context& context)
{
    RenderTarget& render_target = context.render_target_pool.get(light_buffer_render_target_);
    TextureInstance texture;
    render_target.color_texture(texture, 0);
    context.debug_views->add_debug_buffer(string("lighting"), texture, Renderer::renderer_debug_mode_rgb);
}

void GBufferDrawMaterialSystem::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void GBufferDrawMaterialSystem::update(Context& context, SceneContext& /*scene_context*/, RenderContext& render_context)
{
    context.materials[id()].clear();
    clear_command_.reset();

    size_t use_cubemap = 0;
    const TextureInstance& cubemap_texture = render_context.space_grid.global_cubemap();
    if (cubemap_texture.id != Texture::invalid_id)
        use_cubemap = 1;

    for (size_t i = 0; i < render_context.lights_number; ++i)
    {
        int type = 0;
        size_t use_shadowmap = 0;
        TextureInstance shadowmap_texture;
        UniformBuffer::IdType light_uniform = render_context.lights[i].uniform_id;
        // Update light data
        LightData data;
        const res::Light& light = render_context.lights[i].dblight;
        type = light.type;

        const ShadowInfo* shadow_info = render_context.lights[i].shadow_info;
        if (shadowmap_enabled_.value() && shadow_info != nullptr)
        {
            use_shadowmap = 1;
            shadowmap_texture = shadow_info->shadowmap;
        }

        // Choose the correct shader
        UberShader& shader = ubershader(context);
        UberShader::Index ubershader_index;
        ubershader_index.set(shader.info("LIGHT_TYPE"), type);
        ubershader_index.set(shader.info("SHADOWMAP"), use_shadowmap);
        ubershader_index.set(shader.info("SHADOWMAP_QUALITY"), shadowmap_quality_.value());
        ubershader_index.set(shader.info("CUBEMAP"), use_cubemap);
        ShaderProgram::IdType shader_program_id = ubershader(context).get(ubershader_index);

        // Here we go - it's time to kick off something to draw
        DrawCall& draw_call = render_context.draw_calls.add();

        if (type == directional)
            draw_call.render_data = quad_mesh_.mesh.parts[0].render_data;
        else if (type == spot)
            draw_call.render_data = quad_mesh_.mesh.parts[0].render_data;
        else
            draw_call.render_data = sphere_mesh_.mesh.parts[0].render_data;

        Material& material = create_and_get(context.materials[id()]);
        material.shader_program = shader_program_id;
        material.textures[0] = albedo_texture_;
        material.textures[1] = normal_texture_;
        material.textures[2] = accumnulator_texture_;
        material.textures[3] = depth_texture_;
        material.textures[6] = context.renderer->indirect_diffuse_lighting_texture();
        material.textures[7] = context.renderer->indirect_specular_lighting_texture();
        material.textures[shadowmap_texture_unit] = shadowmap_texture;
        material.uniforms[0] = render_context.main_camera.percamera_uniform;
        material.uniforms[2] = light_uniform;
        draw_call.material.material_system = id();
        draw_call.material.id = material.id;

        draw_call.render_state = render_state_;
        draw_call.render_target = light_buffer_render_target_;
        draw_call.command = &list_of_commands_;
    }
}

bool ProbesAccumulatorMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    set_layout(FullscreenLayout::handle);

    if (!init_default(context, material_system_context))
        return false;

    RenderTargetDesc rt_desc;
    rt_desc.target = rt_readwrite;
    rt_desc.color_datatype[0] = format_ubyte;
    rt_desc.color_format[0] = format_rgba;
    rt_desc.color_targets = 1;
    rt_desc.use_depth = false;
    rt_desc.use_stencil = false;
    rt_desc.width = context.window_system.width();
    rt_desc.height = context.window_system.height();

    RenderTarget& render_target = context.render_target_manager.create(context, rt_desc, 1.0f);
    render_target_ = render_target.id();
    render_target.color_texture(output_, 0);

    if (!init_fullscreen_quad(context))
    {
        destroy(context);
        return false;
    }

    return true;
}

void ProbesAccumulatorMaterialSystem::destroy(Context& context)
{
    destroy_pool_object(context.render_state_pool, quad_mesh_.instance_parts[0].render_state_id);
    destroy_mesh_instance(quad_mesh_, context);

    context.render_target_manager.destroy(render_target_, context);
}

void ProbesAccumulatorMaterialSystem::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

bool ProbesAccumulatorMaterialSystem::init_fullscreen_quad(Context& context)
{
    quad_mesh_.mesh.parts.resize(1);
    quad_mesh_.instance_parts.resize(1);

    if (!utils::create_fullscreen_quad(quad_mesh_, context))
    {
        ERROR_LOG("Can't create fullscreen quad");
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
    return true;
}

void ProbesAccumulatorMaterialSystem::update(Context& context, SceneContext& /*scene_context*/, RenderContext& render_context)
{
    // currenty only one cubemap is supported
    if (render_context.space_grid.global_cubemap().id == Texture::invalid_id)
        return;
    context.materials[id()].clear();
    clear_command_.reset();

    Material& material = create_and_get(context.materials[id()]);
    material.shader_program = default_program(context).id();
    material.uniforms[0] = render_context.main_camera.percamera_uniform;
    material.textures[0] = gbuffer_.albedo;
    material.textures[1] = gbuffer_.normal;
    material.textures[2] = gbuffer_.accumulator;
    material.textures[3] = gbuffer_.depth;
    material.textures[4] = render_context.space_grid.global_cubemap();
    quad_mesh_.instance_parts[0].material.id = material.id;
    quad_mesh_.instance_parts[0].material.material_system = id();

    setup_draw_call(render_context.draw_calls.add(), quad_mesh_.instance_parts[0], quad_mesh_.mesh.parts[0], render_target_, &clear_command_);
}

void ProbesAccumulatorMaterialSystem::set_gbuffer(const GBuffer& gbuffer)
{
    gbuffer_ = gbuffer;
}

}
