#ifndef __DRAW_CALL_HPP__
#define __DRAW_CALL_HPP__

#include "node.hpp"
#include "core/array.hpp"

namespace mhe {

class Atomic;

const size_t max_atomics_number = 4;

template <class DC>
inline void prepare_draw_call(DC& draw_call)
{
    ::memset(&draw_call, 0, sizeof(DC));
}

struct DrawCall
{
    RenderStateHandleType render_state;
    RenderTargetHandleType render_target;
    MaterialInstance material;
    RenderData render_data;
    array<Atomic*, max_atomics_number> atomics;
    RenderCommand* command;
    int barrier;
    uint8_t pass;

    DrawCall() : render_state(InvalidHandle<RenderStateHandleType>::id),
        render_target(default_render_target), command(nullptr), barrier(memory_barrier_none), pass(0)
    {
        atomics.fill(nullptr);
    }
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
const size_t compute_call_uniforms_number = 5;

struct ComputeCallExplicit
{
    ShaderProgram* shader_program;
    Texture* images[compute_call_images_number];
    Texture* textures[compute_call_images_number];
    int image_access[compute_call_images_number];
    ShaderStorageBuffer* buffers[compute_call_buffers_number];
    UniformBuffer* uniforms[compute_call_uniforms_number];
    uivec3 workgroups_number;
    int barrier;

    ComputeCallExplicit()
    {
        ::memset(this, 0, sizeof(ComputeCallExplicit));
    }
};

}

#endif // DRAW_CALL_HPP
