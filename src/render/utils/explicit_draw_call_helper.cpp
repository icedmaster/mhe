#include "render/utils/explicit_draw_call_helper.hpp"

#include "render/context.hpp"
#include "render/draw_call.hpp"

namespace mhe {
namespace utils {

void convert(DrawCallExplicit& dst, const DrawCall& src, const Context& context)
{
    prepare_draw_call(dst);

    Material& material = context.materials[src.material.material_system].get(src.material.id);
    const DrawCallData& draw_call_data = src.draw_call_data;

    dst.elements_number = src.render_data.elements_number;
    dst.ibuffer = &context.index_buffer_pool.get(src.render_data.ibuffer);
    dst.ibuffer_offset = src.render_data.ibuffer_offset;
    dst.indices_number = src.render_data.indexes_number;
    dst.layout = &context.layout_pool.get(src.render_data.layout);
    dst.pass = src.pass;
    dst.primitive = src.render_data.primitive;
    dst.priority = 0;
    dst.render_command = src.command;
    dst.render_state = &context.render_state_pool.get(draw_call_data.state);
    if (draw_call_data.render_target != default_render_target)
        dst.render_target = &context.render_target_pool.get(draw_call_data.render_target);
    dst.shader_program = &context.shader_pool.get(material.shader_program);
    for (size_t i = 0; i < material_texture_buffers_number; ++i)
    {
        if (material.texture_buffers[i] == TextureBuffer::invalid_id)
            continue;
        dst.texture_buffers[i] = &context.texture_buffer_pool.get(material.texture_buffers[i]);
    }
    for (size_t i = 0; i < material_textures_number; ++i)
    {
        if (material.textures[i].id == Texture::invalid_id) continue;
        dst.textures[i] = &context.texture_pool.get(material.textures[i].id);
    }
    for (size_t i = 0; i < material_uniforms_number; ++i)
    {
        if (material.uniforms[i] == UniformBuffer::invalid_id) continue;
        dst.uniforms[i] = &context.uniform_pool.get(material.uniforms[i]);
    }
    dst.vbuffer = &context.vertex_buffer_pool.get(src.render_data.vbuffer);
    dst.vbuffer_offset = src.render_data.vbuffer_offset;
}

}}
