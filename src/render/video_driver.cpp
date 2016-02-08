#include "render/video_driver.hpp"

#include "impl/system_factory.hpp"

#include "render/instances.hpp"
#include "render/context.hpp"
#include "render/draw_call.hpp"

#include "debug/profiler.hpp"

namespace mhe {

// helper stats class
void Driver::Stats::update(size_t tris)
{
    ++batches_;
    tris_ += tris;
}

void Driver::Stats::update_frames()
{
    ++frames_;
}

Driver::Driver() :
    current_version_(0),
    impl_(SystemFactory::instance().create_driver())
{
    reset_state();
    versions_number_ = impl_->supported_versions(versions_, ARRAY_SIZE(versions_));
}

void Driver::clear(bool clear_color, bool clear_depth, bool clear_stencil,
        const colorf& color, float depth, uint stencil)
{
    if (clear_color)
    {
        impl_->set_clear_color(color);
        impl_->clear_color();
    }

    if (clear_depth)
    {
        impl_->clear_depth();
        NOT_IMPLEMENTED(depth);
    }

    impl_->clear_stencil();
    NOT_IMPLEMENTED(clear_stencil);
    NOT_IMPLEMENTED(stencil);
}

void Driver::reset()
{
    stats_.reset();
    impl_.reset(SystemFactory::instance().create_driver());
}

void Driver::render(Context& context, const DrawCall* draw_calls, size_t count)
{
    ProfilerElement pe("driver.render");
    for (size_t i = 0; i < count; ++i)
        perform_draw_call(context, draw_calls[i]);
    if (state_.last_command != nullptr && state_.last_command->stages() & render_stage_end_priority)
        state_.last_command->execute(context, render_stage_end_priority);
}

void Driver::render(Context& context, const DrawCallExplicit* draw_calls, size_t count)
{
    ProfilerElement pe("driver.render_explicit");
    for (size_t i = 0; i < count; ++i)
        perform_draw_call(context, draw_calls[i]);
}

void Driver::perform_draw_call(Context& context, const DrawCallExplicit& draw_call)
{
    if (draw_call.render_target != nullptr)
    {
        if (state_.render_target != draw_call.render_target->id())
        {
            state_.render_target = draw_call.render_target->id();
            impl_->set_render_target(*draw_call.render_target);
        }
    }
    else
    {
        if (state_.render_target != default_render_target)
        {
            state_.render_target = default_render_target;
            impl_->set_default_render_target();
        }
    }

    uint8_t render_command_stages = draw_call.render_command != nullptr ? draw_call.render_command->stages() : 0;

    if (render_command_stages & render_stage_after_render_target_setup)
        draw_call.render_command->execute(context, render_stage_after_render_target_setup);

    ASSERT(draw_call.render_state != nullptr, "Invalid RenderState");
    if (state_.state != draw_call.render_state->id())
    {
        impl_->set_state(*draw_call.render_state);
        state_.state = draw_call.render_state->id();
    }

    if (render_command_stages & render_stage_before_submit)
    {
        if (!draw_call.render_command->execute(context, render_stage_before_submit))
            return;
    }

    bool shader_program_changed = false;
    bool layout_changed = false;
    ASSERT(draw_call.shader_program != nullptr, "Invalid ShaderProgram");
    if (state_.shader_program != draw_call.shader_program->id())
    {
        impl_->set_shader_program(*draw_call.shader_program);
        state_.shader_program = draw_call.shader_program->id();
        shader_program_changed = true;
        layout_changed = true;
    }

    ASSERT(draw_call.vbuffer != nullptr, "Invalid VertexBuffer");
    if (state_.vertex_buffer != draw_call.vbuffer->id())
    {
        impl_->set_vertex_buffer(*draw_call.vbuffer);
        state_.vertex_buffer = draw_call.vbuffer->id();
        layout_changed = true;
    }

    ASSERT(draw_call.ibuffer != nullptr, "Invalid IndexBuffer");
    if (state_.index_buffer != draw_call.ibuffer->id())
    {
        impl_->set_index_buffer(*draw_call.ibuffer);
        state_.index_buffer = draw_call.ibuffer->id();
        layout_changed = true;
    }

    ASSERT(draw_call.layout != nullptr, "Invalid Layout");
    if (state_.layout != draw_call.layout->id() || layout_changed)
    {
        impl_->set_layout(*draw_call.layout);
        state_.layout = draw_call.layout->id();
    }

    for (size_t j = 0; j < material_textures_number; ++j)
    {
        if (draw_call.textures[j] == nullptr || draw_call.textures[j]->id() == state_.textures[j])
            continue;
        impl_->set_texture(*draw_call.textures[j], j);
        state_.textures[j] = draw_call.textures[j]->id();
    }
    for (size_t j = 0; j < material_uniforms_number; ++j)
    {
        if (draw_call.uniforms[j] == nullptr || (draw_call.uniforms[j]->id() == state_.uniforms[j] && !shader_program_changed))
            continue;
        impl_->set_uniform(*draw_call.uniforms[j], j);
        state_.uniforms[j] = draw_call.uniforms[j]->id();
    }
    for (size_t j = 0; j < material_texture_buffers_number; ++j)
    {
        if (draw_call.texture_buffers[j] == nullptr || (draw_call.texture_buffers[j]->id() == state_.texture_buffers[j] && !shader_program_changed))
            continue;
        impl_->set_texture_buffer(*draw_call.texture_buffers[j], j);
        state_.texture_buffers[j] = draw_call.texture_buffers[j]->id();
    }
    impl_->draw(draw_call.elements_number, draw_call.vbuffer_offset, draw_call.ibuffer_offset, draw_call.indices_number, draw_call.primitive);
    stats_.update(draw_call.elements_number);
}

void Driver::perform_draw_call(Context& context, const DrawCall& draw_call)
{
    uint8_t material_system_id = draw_call.material.material_system;
    uint8_t material_system_priority = context.material_systems.get(material_system_id)->priority();

    uint8_t command_render_stages = draw_call.command != nullptr ? draw_call.command->stages() : 0;

    if (material_system_priority != state_.priority)
    {
        if (state_.last_command != nullptr && state_.last_command->stages() & render_stage_end_priority)
            state_.last_command->execute(context, render_stage_end_priority);
        if (command_render_stages & render_stage_begin_priority)
            draw_call.command->execute(context, render_stage_begin_priority);
        state_.priority = material_system_priority;
    }
    state_.last_command = draw_call.command;

    if (command_render_stages & render_stage_before_render_target_setup)
        draw_call.command->execute(context, render_stage_before_render_target_setup);

    if (draw_call.render_target != default_render_target)
    {
        if (state_.render_target != draw_call.render_target)
        {
            state_.render_target = draw_call.render_target;
            impl_->set_render_target(context.render_target_pool.get(draw_call.render_target));
        }
    }
    else
    {
        if (state_.render_target != default_render_target)
        {
            state_.render_target = default_render_target;
            impl_->set_default_render_target();
        }
    }

    if (command_render_stages & render_stage_after_render_target_setup)
        draw_call.command->execute(context, render_stage_after_render_target_setup);

    if (state_.state != draw_call.render_state)
    {
        impl_->set_state(context.render_state_pool.get(draw_call.render_state));
        state_.state = draw_call.render_state;
    }

    if (command_render_stages & render_stage_before_submit)
    {
        if (!draw_call.command->execute(context, render_stage_before_submit))
            return;
    }

    Material::IdType material_id = draw_call.material.id;
    const Material& material =
        context.materials[material_system_id].get(material_id);

    bool shader_program_changed = false;
    bool layout_changed = false;
    if (state_.shader_program != material.shader_program)
    {
        impl_->set_shader_program(context.shader_pool.get(material.shader_program));
        state_.shader_program = material.shader_program;
        shader_program_changed = true;
        layout_changed = true;
    }

    if (state_.vertex_buffer != draw_call.render_data.vbuffer)
    {
        impl_->set_vertex_buffer(context.vertex_buffer_pool.get(draw_call.render_data.vbuffer));
        state_.vertex_buffer = draw_call.render_data.vbuffer;
        layout_changed = true;
    }

    if (state_.index_buffer != draw_call.render_data.ibuffer)
    {
        impl_->set_index_buffer(context.index_buffer_pool.get(draw_call.render_data.ibuffer));
        state_.index_buffer = draw_call.render_data.ibuffer;
        layout_changed = true;
    }

    if (state_.layout != draw_call.render_data.layout || layout_changed)
    {
        impl_->set_layout(context.layout_pool.get(draw_call.render_data.layout));
        state_.layout = draw_call.render_data.layout;
    }

    for (size_t j = 0; j < material_textures_number; ++j)
    {
        if (material.textures[j].id == Texture::invalid_id || material.textures[j].id == state_.textures[j])
            continue;
        const Texture& texture = context.texture_pool.get(material.textures[j].id);
        impl_->set_texture(texture, j);
        state_.textures[j] = material.textures[j].id;
    }
    for (size_t j = 0; j < material_uniforms_number; ++j)
    {
        if (material.uniforms[j] == UniformBuffer::invalid_id || (material.uniforms[j] == state_.uniforms[j] && !shader_program_changed))
            continue;
        const UniformBuffer& uniform = context.uniform_pool.get(material.uniforms[j]);
        impl_->set_uniform(uniform, j);
        state_.uniforms[j] = material.uniforms[j];
    }
    for (size_t j = 0; j < material_texture_buffers_number; ++j)
    {
        if ((material.texture_buffers[j] == TextureBuffer::invalid_id) || (material.texture_buffers[j] == state_.texture_buffers[j] && !shader_program_changed))
            continue;
        const TextureBuffer& texture_buffer = context.texture_buffer_pool.get(material.texture_buffers[j]);
        impl_->set_texture_buffer(texture_buffer, j);
        state_.texture_buffers[j] = texture_buffer.id();
    }
    impl_->draw(draw_call.render_data);
    stats_.update(draw_call.render_data.elements_number);

    if (command_render_stages & render_stage_after_submit)
        draw_call.command->execute(context, render_stage_after_submit);
}

void Driver::execute(Context& context, const ComputeCallExplicit* compute_calls, size_t count)
{
    for (size_t i = 0; i < count; ++i)
        perform_compute_call(context, compute_calls[i]);
}

void Driver::memory_barrier(uint32_t barriers)
{
    impl_->memory_barrier(barriers);
}

void Driver::perform_compute_call(Context& /*context*/, const ComputeCallExplicit& compute_call)
{
    ASSERT(compute_call.shader_program != nullptr, "Invalid shader program");
    impl_->set_shader_program(*compute_call.shader_program);
    for (size_t i = 0; i < compute_call_images_number; ++i)
    {
        if (compute_call.images[i] == nullptr) continue;
        impl_->set_image(*compute_call.images[i], i, compute_call.image_access[i]);
    }
    for (size_t i = 0; i < compute_call_buffers_number; ++i)
    {
        if (compute_call.buffers[i] == nullptr) continue;
        impl_->set_shader_storage_buffer(*compute_call.buffers[i], i);
    }
    for (size_t i = 0; i < compute_call_buffers_number; ++i)
    {
        if (compute_call.uniforms[i] == nullptr) continue;
        impl_->bind_uniform(*compute_call.uniforms[i], i);
    }
    impl_->dispatch(compute_call.workgroups_number.x(), compute_call.workgroups_number.y(), compute_call.workgroups_number.z());
}

void Driver::begin_render()
{
    impl_->set_default_render_target();
}

void Driver::end_render()
{
    stats_.update_frames();
    impl_->flush();

    reset_state();
}

void Driver::reset_state()
{
    ::memset(&state_, 0xff, sizeof(state_));
    state_.last_command = nullptr;
}

}
