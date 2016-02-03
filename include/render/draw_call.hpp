#ifndef __DRAW_CALL_HPP__
#define __DRAW_CALL_HPP__

#include "node.hpp"

namespace mhe {

struct DrawCall
{
    DrawCallData draw_call_data;
    MaterialInstance material;
    RenderData render_data;
    RenderCommand* command;
    uint8_t pass;

    DrawCall() : command(nullptr), pass(0) {}
};

// TODO: move Explicit draw calls to a separate header (can use forward declaration then)

struct DrawCallExplicit
{
    RenderState* render_state;
    RenderTarget* render_target;
    Texture* textures[material_textures_number];
    UniformBuffer* uniforms[material_uniforms_number];
    TextureBuffer* texture_buffers[material_texture_buffers_number];
    ShaderProgram* shader_program;
    size_t elements_number;
    uint32_t vbuffer_offset;
    uint32_t ibuffer_offset;
    size_t indices_number;
    Primitive primitive;
    Layout* layout;
    IndexBuffer* ibuffer;
    VertexBuffer* vbuffer;
    RenderCommand* render_command;
    uint8_t pass;
    // Will be moved to the ID
    uint8_t priority;
};

const size_t compute_call_images_number = 8;
const size_t compute_call_buffers_number = 4;

struct ComputeCallExplicit
{
    ShaderProgram* shader_program;
    Texture* images[compute_call_images_number];
    int image_access[compute_call_images_number];
    ShaderStorageBuffer* buffers[compute_call_buffers_number];
    UniformBuffer* uniforms[compute_call_buffers_number];
    uivec3 workgroups_number;
};

template <class DC>
inline void prepare_draw_call(DC& draw_call)
{
    ::memset(&draw_call, 0, sizeof(DC));
}

}

#endif // DRAW_CALL_HPP
