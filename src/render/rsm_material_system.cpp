#include "render/rsm_material_system.hpp"

#include "render/context.hpp"
#include "render/render_context.hpp"
#include "render/light.hpp"
#include "render/light_instance_methods.hpp"
#include "render/uniforms.hpp"
#include "utils/global_log.hpp"
#include "debug/debug_views.hpp"

namespace mhe {

RSMMaterialSystem::RSMMaterialSystem() :
    profiler_command_("rsm")
{}

bool RSMMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    if (!MaterialSystem::init_default(context, material_system_context))
    {
        ERROR_LOG("Can't initialize the RSMMaterialSystem");
        return false;
    }

    // GBuffer
    pool_initializer<RenderTargetPool> rt_wr(context.render_target_pool);
    RenderTargetDesc rt_desc;
    rt_desc.color_targets = 2;
    rt_desc.color_format[0] = format_rgb16f; // normals
    rt_desc.color_format[1] = format_rgb16f; // accumulator
    rt_desc.color_datatype[0] = format_default;
    rt_desc.color_datatype[1] = format_default;
    rt_desc.texture_type = texture_2d;
    rt_desc.target = rt_readwrite;
    rt_desc.height = settings_.size;
    rt_desc.width = settings_.size;
    rt_desc.use_depth = true;
    rt_desc.use_stencil = false;
    rt_desc.depth_format = format_d24f;
    rt_desc.depth_datatype = format_default;
    
    RenderTarget& render_target = rt_wr.object();
    if (!render_target.init(context, rt_desc))
    {
        ERROR_LOG("Can't initialize a GBuffer for RSMMaterialSystem");
        return false;
    }

    pool_initializer<RenderStatePool> render_state_wr(context.render_state_pool);
    RenderStateDesc render_state_desc;
    render_state_desc.viewport.viewport.set(0, 0, settings_.size, settings_.size);
    if (!render_state_wr.object().init(render_state_desc))
    {
        ERROR_LOG("Can't initialize a render state for RSMMaterialSystem");
        return false;
    }

    pool_initializer<UniformPool> uniform_wr(context.uniform_pool);
    UniformBufferDesc uniform_desc;
    uniform_desc.unit = 0;
    uniform_desc.update_type = uniform_buffer_normal;
    uniform_desc.size = sizeof(ShaderData);
    if (!uniform_wr.object().init(uniform_desc))
    {
        ERROR_LOG("Can't initialize a uniform for RSMMaterialSystem");
        return false;
    }

    transform_uniform_ = uniform_wr.take();
    render_target_ = rt_wr.take();
    render_state_ = render_state_wr.take();

    TextureInstance gbuffer_textures[2];
    render_target.color_textures(gbuffer_textures);
    gbuffer_.normal = gbuffer_textures[0];
    gbuffer_.accumulator = gbuffer_textures[1];
    render_target.depth_texture(gbuffer_.depth);

    list_of_commands_.add_command(&clear_command_);
    list_of_commands_.add_command(&profiler_command_);
    profiler_command_.set_stages(render_stage_begin_priority | render_stage_end_priority);

    return true;
}

void RSMMaterialSystem::destroy(Context& context)
{
    destroy_pool_object(context.render_state_pool, render_state_);
    destroy_pool_object(context.uniform_pool, transform_uniform_);
    destroy_pool_object(context.render_target_pool, render_target_);
}

void RSMMaterialSystem::setup(Context &context, SceneContext &scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void RSMMaterialSystem::start_frame(Context& /*context*/, SceneContext& scene_context, RenderContext& render_context)
{
    // Currently, we're working with the first directional light
    LightInstance* light_instance = nullptr;
    LightInstance* lights = render_context.lights;
    for (size_t i = 0; i < render_context.lights_number; ++i)
    {
        if (lights[i].dblight.type == directional)
        {
            light_instance = &lights[i];
            break;
        }
    }

    if (light_instance == nullptr) return;

    light_direction_ = get_light_direction(scene_context, light_instance->id);

    const AABBf& aabb = settings_.mode == mode_scene ? render_context.aabb : settings_.volume_aabb;
    current_volume_ = aabb;

    float radius = aabb.extents.magnitude();
    vec3 light_position = aabb.center - light_direction_ * radius;
    light_view_ = get_light_view_matrix(scene_context, light_instance->id);
    light_view_.set_row(3, vec3::zero());
    light_view_.multTranslate(-light_position);

    light_proj_.set_ortho(-radius, radius, -radius, radius, 0.0f, radius * 2.0f);

    light_vp_ = light_view_ * light_proj_;

    FrustumCullingRequest frustum_culling_request;
    frustum_culling_request.vp = light_vp_;
    render_context.render_view_requests.register_request(additional_view0, frustum_culling_request);

    shader_data_.vp = light_vp_;
    shader_data_.settings.set_x(light_instance->dblight.intensity * settings_.flux_intensity);
    shader_data_.light_diffuse_color = light_instance->dblight.diffuse_color;
}

void RSMMaterialSystem::update(Context& context, SceneContext& /*scene_context*/, RenderContext& render_context)
{
    context.uniform_pool.get(transform_uniform_).update(shader_data_);

    context.materials[id()].clear();
    clear_command_.reset();

    UberShader& main_ubershader = ubershader(context);
    const UberShader::Info& skinning_info = main_ubershader.info("SKINNING");
    const UberShader::Info& normalmap_info = main_ubershader.info("NORMALMAP");

    const bool* visibility = render_context.render_view_requests.frustum_culling_request_data(additional_view0).result.visibility.data();
    NodeInstance* nodes = render_context.nodes;
    size_t v = 0;
    for (size_t i = 0; i < render_context.nodes_number; ++i)
    {
        for (size_t j = 0, size = nodes[i].mesh.instance_parts.size(); j < size; ++j, ++v)
        {
            if (!visibility[v]) continue;

            MeshPartInstance& mesh_part = nodes[i].mesh.instance_parts[j];

            Material& original_material = context.materials[mesh_part.material.material_system].get(mesh_part.material.id);
            
            Material& material = create_and_get(context.materials[id()]);
            material.textures[albedo_texture_unit] = original_material.textures[albedo_texture_unit];
            material.textures[normal_texture_unit] = original_material.textures[normal_texture_unit];
            material.uniforms[0] = transform_uniform_;
            material.uniforms[1] = original_material.uniforms[1];
            material.texture_buffers[animation_texture_unit] = original_material.texture_buffers[animation_texture_unit];

            UberShader::Index shader_index;
            shader_index.set(skinning_info, is_handle_valid(original_material.texture_buffers[animation_texture_unit]) ? 1 : 0);
            shader_index.set(normalmap_info, is_handle_valid(material.textures[normal_texture_unit].id) ? 1 : 0);
            material.shader_program = main_ubershader.get(shader_index);

            DrawCall& draw_call = render_context.draw_calls.add();
            draw_call.material.id = material.id;
            draw_call.material.material_system = id();
            draw_call.pass = 0;
            draw_call.render_data = nodes[i].mesh.mesh.parts[j].render_data;
            draw_call.render_state = render_state_;
            draw_call.render_target = render_target_;
            draw_call.command = &list_of_commands_;
        }
    }
}

void RSMMaterialSystem::init_debug_views(Context& context)
{
    size_t view_id = context.debug_views->add_view("RSM");
    DebugViews::DebugView& view = context.debug_views->get_view(view_id);
    view.add("flux_coeff", 0.0f, 2.0f, &settings_.flux_intensity);
}

}
