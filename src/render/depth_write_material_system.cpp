#include "render/depth_write_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/render_context.hpp"
#include "render/scene_context.hpp"
#include "render/instances.hpp"
#include "render/light_instance_methods.hpp"

namespace mhe {

bool DepthWriteMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
    set_layout(StandartGeometryLayout::handle);

    if (!init_default(context, material_system_context))
        return false;

    // It seems we can't pass a static member as a reference
    // (http://stackoverflow.com/questions/272900/vectorpush-back-odr-uses-the-value-causing-undefined-reference-to-static-clas?lq=1)
    uniforms_.fill(static_cast<UniformBuffer::IdType>(UniformBuffer::invalid_id));
    shadowmaps_.fill(TextureInstance());
    render_states_.fill(static_cast<RenderState::IdType>(RenderState::invalid_id));
    render_targets_.fill(static_cast<RenderTarget::IdType>(RenderTarget::invalid_id));

    return true;
}

bool DepthWriteMaterialSystem::init_light_data(Context& context)
{
    UniformBuffer::IdType& next_id = uniforms_.add();
    if (next_id == UniformBuffer::invalid_id)
    {
        next_id = context.uniform_pool.create();
        UniformBuffer& uniform_buffer = context.uniform_pool.get(next_id);
        UniformBufferDesc desc;
        desc.name = "transform";
        desc.program = &(default_program(context));
        if (!uniform_buffer.init(desc))
            return false;
    }

    RenderStateHandleType& next_render_state = render_states_.add();
    if (!is_handle_valid(next_render_state))
    {
        RenderState& render_state = create_and_get(context.render_state_pool);
        RenderStateDesc render_state_desc;
        render_state_desc.viewport.viewport.set(0, 0, shadowmap_default_width, shadowmap_default_height);
        if (!render_state.init(render_state_desc)) return false;
        next_render_state = render_state.id();

        RenderTarget& render_target = create_and_get(context.render_target_pool);
        RenderTargetDesc desc;
        desc.target = rt_readwrite;
        desc.color_targets = 0;
        desc.use_depth = true;
        desc.use_stencil = false;
        desc.depth_datatype = format_float;
        desc.depth_format = format_d24f;
        desc.width = shadowmap_default_width;
        desc.height = shadowmap_default_height;
        if (!render_target.init(context, desc)) return false;

        render_targets_.push_back(render_target.id());

        TextureInstance shadowmap;
        render_target.depth_texture(shadowmap);
        shadowmaps_.push_back(shadowmap);
    }

    return true;
}

void DepthWriteMaterialSystem::destroy(Context&)
{
}

void DepthWriteMaterialSystem::setup(Context& /*context*/, SceneContext& /*scene_context*/, MeshPartInstance* /*parts*/, MeshPart* /*part*/, ModelContext* /*model_contexts*/, size_t /*count*/)
{
    ASSERT(0, "This method should not be called");
}

void DepthWriteMaterialSystem::update(Context& context, SceneContext& scene_context, RenderContext& render_context)
{
    // clear() resets only counter but doesn't really destroy the elements of the vector
    clear_commands_.clear();
    uniforms_.clear();
    shadow_info_.clear();
    render_states_.clear();

    context.materials[id()].clear();

    bool have_shadowcasters = false;
    for (size_t i = 0; i < render_context.lights_number; ++i)
    {
        if (!render_context.lights[i].dblight.cast_shadows || render_context.lights[i].shadow_info != nullptr)
            continue;
        if (!init_light_data(context))
        {
            ERROR_LOG("Shadowmap data initialization failed");
            return;
        }
        size_t index = uniforms_.size() - 1;
        UniformBuffer& uniform = context.uniform_pool.get(uniforms_[index]);
        UniformData data;
        data.mvp = get_light_shadowmap_matrix(scene_context, render_context.lights[i].id);
        uniform.update(data);

        ShadowInfo& shadow_info = shadow_info_.add();
        shadow_info.shadowmap = shadowmaps_[index];
        shadow_info.lightvp = data.mvp;
        shadow_info.lightview = data.mvp;
        shadow_info.cascades_number = 1;
        shadow_info.offset[0] = vec3::zero();
        shadow_info.scale[0] = vec3(1.0f, 1.0f, 1.0f);
        render_context.lights[i].shadow_info = &shadow_info;

        ClearCommand& command = clear_commands_.add();
        command.reset();

        have_shadowcasters = true;
    }

    if (!have_shadowcasters) return;

    for (size_t l = 0, light_data_index = 0; l < render_context.lights_number; ++l)
    {
        if (!render_context.lights[l].dblight.cast_shadows)
            continue;
        for (size_t i = 0; i < render_context.nodes_number; ++i)
        {
            if (!render_context.nodes[i].cast_shadow && !render_context.nodes[i].receive_shadow)
                continue;

            for (size_t j = 0; j < render_context.nodes[i].mesh.instance_parts.size(); ++j)
            {
                const MeshPartInstance& part = render_context.nodes[i].mesh.instance_parts[j];
                DrawCall& draw_call = render_context.draw_calls.add();
                draw_call.render_data = render_context.nodes[i].mesh.mesh.parts[j].render_data;
                Material& material = create_and_get(context.materials[id()]);
                material.shader_program = default_program(context).id();
                material.uniforms[0] = uniforms_[light_data_index];
                material.id = material.id;
                draw_call.material.material_system = id();
                draw_call.material.id = material.id;

                const MaterialInstance& original_material_instance = part.material;
                const Material& original_material = context.materials[original_material_instance.material_system].get(original_material_instance.id);
                material.uniforms[1] = original_material.uniforms[1];

                draw_call.render_state = render_states_[light_data_index];
                draw_call.render_target = render_targets_[light_data_index];

                draw_call.command = &clear_commands_[light_data_index];
                draw_call.pass = light_data_index;
            }
        }
        ++light_data_index;
    }
}

}
