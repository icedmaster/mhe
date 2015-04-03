#include "render/posteffect_material_system.hpp"

#include "render/layouts.hpp"
#include "render/context.hpp"
#include "render/instances.hpp"
#include "render/utils/simple_meshes.hpp"
#include "render/render_context.hpp"

namespace mhe {

bool PosteffectSimpleMaterialSystem::init(Context& context, const MaterialSystemContext& material_system_context)
{
	set_layout(FullscreenLayout::handle);

    if (!context.shader_manager.get(shader(), material_system_context.shader_name))
    {
        ERROR_LOG("Can't get shader:" << material_system_context.shader_name);
        return false;
    }

    return init_mesh(context);
}

bool PosteffectSimpleMaterialSystem::init_mesh(Context& context)
{
    mesh_.mesh.parts.resize(1);
    mesh_.instance_parts.resize(1);
    if (!utils::create_fullscreen_quad(mesh_, context))
    {
        ERROR_LOG("Can't create fullscreen quad");
        return false;
    }
    mesh_.mesh.parts[0].render_data.layout = layout();

    DrawCallData& draw_call_data = create_and_get(context.draw_call_data_pool);
    RenderState& render_state = create_and_get(context.render_state_pool);

    draw_call_data.state = render_state.id();
    RenderStateDesc desc;
    desc.blend.enabled = true;
    desc.blend.srcmode = blend_src_alpha;
    desc.blend.dstmode = blend_src_inv_alpha;
    desc.depth.enabled = false;
    render_state.init(desc);

    mesh_.instance_parts[0].draw_call_data = draw_call_data.id;

    Material& material = create_and_get(context.materials[id()]);
    material.shader_program = default_program(context).id();
    for (size_t j = 0; j < material_textures_number; ++j)
    {
        if (textures_[j].id == Texture::invalid_id)
            continue;
        material.textures[j] = textures_[j];
    }
    mesh_.instance_parts[0].material.material_system = id();
    mesh_.instance_parts[0].material.id = material.id;
    return true;
}

void PosteffectSimpleMaterialSystem::close()
{
}

void PosteffectSimpleMaterialSystem::setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count)
{
    empty_setup(context, scene_context, instance_parts, parts, model_contexts, count);
}

void PosteffectSimpleMaterialSystem::update(Context& context, SceneContext& /*scene_context*/, RenderContext& render_context)
{
    Material& material = context.materials[id()].get(mesh_.instance_parts[0].material.id);
    for (size_t j = 0; j < material_textures_number; ++j)
    {
        if (textures_[j].id == Texture::invalid_id)
            continue;
        material.textures[j] = textures_[j];
    }
	material.uniforms[0] = render_context.percamera_uniform;
    setup_draw_call(render_context.draw_calls.add(), mesh_.instance_parts[0], mesh_.mesh.parts[0]);
}

}
