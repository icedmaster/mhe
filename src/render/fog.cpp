#include "render/fog.hpp"

#include "render/context.hpp"
#include "utils/global_log.hpp"

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
    shader_data.param1 = vec4(settings_.start, settings_.density, 0.0f, 0.0f);
    shader_data.fog_color = settings_.color;
    UniformBuffer& uniform = context.uniform_pool.get(uniform_buffer_id_);
    uniform.update(shader_data);

    PosteffectMaterialSystemBase::update(context, scene_context, render_context);
}

}
