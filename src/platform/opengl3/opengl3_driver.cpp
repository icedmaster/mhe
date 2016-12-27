#include "platform/opengl3/opengl3_driver.hpp"

#include "platform/opengl/mhe_gl.hpp"
#include "platform/opengl/opengl_extension.hpp"
#include "platform/opengl/opengl_types.hpp"
#include "platform/opengl/opengl_helpers.hpp"

#include "platform/opengl3/opengl3_shader_program.hpp"
#include "platform/opengl3/opengl3_render_state.hpp"
#include "platform/opengl3/opengl3_buffer.hpp"
#include "platform/opengl3/opengl3_texture.hpp"
#include "platform/opengl3/opengl3_render_target.hpp"
#include "platform/opengl3/opengl3_atomics.hpp"

namespace mhe {
namespace opengl {

OpenGL3ContextState::OpenGL3ContextState() :
    cull_mode(GL_BACK), cull_winding(GL_CCW), depth_test(false), depth_write(false), stencil(false), blend(false), scissor_test(false), cull(true),
    rasterizer_depth_test_disabled(false), color_mask(0xf)
{
    uniforms.fill(UniformBuffer::invalid_id);
}

OpenGL3Driver::OpenGL3Driver()
{
    versions_[0].first = 4; versions_[0].second = 5;
    versions_[1].first = 4; versions_[1].second = 3;
    versions_[2].first = 4; versions_[2].second = 2;
    versions_[3].first = 3; versions_[3].second = 3;
}

bool OpenGL3Driver::init(DriverRenderingCapabilities& caps)
{
    // print information about renderer and context
    INFO_LOG("OpenGL3Driver::renderer:" << reinterpret_cast<const char*>(glGetString(GL_RENDERER))); 
    INFO_LOG("OpenGL3Driver::vendor:" << reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    INFO_LOG("OpenGL3Driver::version:" << reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    int major = 0, minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    INFO_LOG("OpenGL3Driver::context:" << major << "." << minor);
    OpenGLExtensions::create_singleton();
    OpenGLExtensions::instance().init_extensions();
    init_text_description();

    current_render_target_ = nullptr;

    setup_caps(caps);

#ifdef MHE_OPENGL_USE_SRGB
    glEnable(GL_FRAMEBUFFER_SRGB);
#endif

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    return true;
}

void OpenGL3Driver::close()
{
    OpenGLExtensions::destroy_singleton();
}

void OpenGL3Driver::setup_caps(DriverRenderingCapabilities& caps)
{
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &caps.max_anisotropic_level);
}

void OpenGL3Driver::clear_depth()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenGL3Driver::clear_color()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGL3Driver::clear_stencil()
{
    glClear(GL_STENCIL_BUFFER_BIT);
}

void OpenGL3Driver::set_clear_color(const colorf& color)
{
    glClearColor(color.r(), color.g(), color.b(), color.a());
}
    
void OpenGL3Driver::enable_blending()
{
    glEnable(GL_BLEND);
}

void OpenGL3Driver::disable_blending()
{
    glDisable(GL_BLEND);
}

void OpenGL3Driver::set_viewport(int x, int y, int w, int h)
{
    glViewport(x, y, w, h);
    state_.viewport.set(x, y, w, h);
}

void OpenGL3Driver::flush()
{
#ifndef PROFILER_ENABLED
    glFlush();
#else
    glFinish();
#endif
}

void OpenGL3Driver::set_state(const RenderState& state)
{
    const OpenGL3RenderState* active_state = static_cast<const OpenGL3RenderState*>(state.impl());
    active_state->enable(state_);
    CHECK_GL_ERRORS();
}

void OpenGL3Driver::set_shader_program(const ShaderProgram& program)
{
    current_shader_program_ = static_cast<const OpenGL3ShaderProgram*>(program.impl());
    current_shader_program_->set();
    CHECK_GL_ERRORS();
}

void OpenGL3Driver::set_vertex_buffer(const RenderBuffer& vbuffer)
{
    const OpenGL3Buffer* buffer = static_cast<const OpenGL3Buffer*>(vbuffer.impl());
    buffer->enable();
    CHECK_GL_ERRORS();
}

void OpenGL3Driver::set_index_buffer(const IndexBuffer& ibuffer)
{
    const OpenGL3IndexBuffer* buffer = static_cast<const OpenGL3IndexBuffer*>(ibuffer.impl());
    current_index_buffer_ = buffer;
    buffer->enable();
}

void OpenGL3Driver::set_uniform(const UniformBuffer& uniform, size_t unit)
{
    const OpenGL3UniformBuffer* buffer = static_cast<const OpenGL3UniformBuffer*>(uniform.impl());
    if (state_.uniforms[unit] != uniform.id())
    {
        buffer->bind(unit);
        state_.uniforms[unit] = uniform.id();
    }
    buffer->enable(current_shader_program_, unit);
    CHECK_GL_ERRORS();
}

void OpenGL3Driver::bind_uniform(const UniformBuffer& uniform, size_t unit)
{
    const OpenGL3UniformBuffer* buffer = static_cast<const OpenGL3UniformBuffer*>(uniform.impl());
    // TODO: need to add state checks
    buffer->bind(current_shader_program_, unit);
    CHECK_GL_ERRORS();
}

void OpenGL3Driver::set_layout(const Layout& layout)
{
    const OpenGL3Layout* active_layout = static_cast<const OpenGL3Layout*>(layout.impl());
    active_layout->enable();
    CHECK_GL_ERRORS();
}

void OpenGL3Driver::set_texture(const Texture& texture, size_t unit)
{
    const OpenGL3Texture* tex = static_cast<const OpenGL3Texture*>(texture.impl());
    tex->enable(unit);
    CHECK_GL_ERRORS();
}

void OpenGL3Driver::set_render_target(const RenderTarget& render_target)
{
    current_render_target_ = static_cast<const OpenGL3RenderTarget*>(render_target.impl());
    current_render_target_->enable();
    CHECK_GL_ERRORS();
}

void OpenGL3Driver::set_texture_buffer(const TextureBuffer& texture_buffer, size_t unit)
{
    const OpenGL3TextureBuffer* buffer = static_cast<const OpenGL3TextureBuffer*>(texture_buffer.impl());
    buffer->enable(current_shader_program_, unit);
    CHECK_GL_ERRORS();
}

void OpenGL3Driver::set_default_render_target()
{
    if (current_render_target_ != nullptr)
    {
        current_render_target_->disable();
        CHECK_GL_ERRORS();
    }
    current_render_target_ = nullptr;
}

void OpenGL3Driver::draw(const RenderData& data)
{
    if (data.primitive == gpu_generated)
    {
        glDrawArrays(GL_POINTS, 0, data.elements_number);
        return;
    }
#ifdef MHE_OPENGL_USE_GL_DRAW_ELEMENTS_BASE_VERTEX
    OpenGLExtensions::instance().glDrawElementsBaseVertex(get_primitive_type(data.primitive), 
        data.indexes_number != 0 ? data.indexes_number : current_index_buffer_->size(), GL_UNSIGNED_INT,
        (void*)(data.ibuffer_offset * sizeof(uint32_t)), data.vbuffer_offset);
#else
    glDrawElements(get_primitive_type(data.primitive),
        data.indexes_number != 0 ? data.indexes_number : current_index_buffer_->size(), GL_UNSIGNED_INT,
        (void*)(data.ibuffer_offset * sizeof(uint32_t)));
#endif
    CHECK_GL_ERRORS();
}

void OpenGL3Driver::draw(size_t elements_number, size_t /*vbuffer_offset*/, size_t ibuffer_offset, size_t indices_number, Primitive primitive)
{
    if (primitive == gpu_generated)
    {
        glDrawArrays(GL_POINTS, 0, elements_number);
        return;
    }
    if (indices_number == 0)
        return;

#ifdef MHE_OPENGL_USE_GL_DRAW_ELEMENTS_BASE_VERTEX
    OpenGLExtensions::instance().glDrawElementsBaseVertex(get_primitive_type(primitive), 
        indices_number != 0 ? indices_number : current_index_buffer_->size(), GL_UNSIGNED_INT,
        (void*)(ibuffer_offset * sizeof(uint32_t)), vbuffer_offset);
#else
    glDrawElements(get_primitive_type(primitive),
        indices_number != 0 ? indices_number : current_index_buffer_->size(), GL_UNSIGNED_INT,
        (void*)(ibuffer_offset * sizeof(uint32_t)));
#endif
    CHECK_GL_ERRORS();  
}

uint OpenGL3Driver::supported_versions(pair<uint, uint>* versions, uint size) const
{
    uint n = min(size, ARRAY_SIZE(versions_));
    for (uint i = 0; i < n; ++i)
        versions[i] = versions_[i];
    return n;
}

void OpenGL3Driver::set_image(const Texture& texture, size_t unit, int access)
{
    const OpenGL3Texture* opengl_texture = static_cast<const OpenGL3Texture*>(texture.impl());
    OpenGLExtensions::instance().glBindImageTexture(unit, opengl_texture->id(), 0, opengl_texture->is_layered() ? GL_TRUE : GL_FALSE,
        0, get_access(access), opengl_texture->image_format());
    CHECK_GL_ERRORS();
}

void OpenGL3Driver::set_shader_storage_buffer(const ShaderStorageBuffer& buffer, size_t unit)
{
    const OpenGL3ShaderStorageBuffer* opengl_buffer = static_cast<const OpenGL3ShaderStorageBuffer*>(buffer.impl());
    opengl_buffer->enable(unit);
}

void OpenGL3Driver::dispatch(size_t x, size_t y, size_t z)
{
    OpenGLExtensions::instance().glDispatchCompute(x, y, z);
    CHECK_GL_ERRORS();
}

void OpenGL3Driver::memory_barrier(uint32_t barriers)
{
    OpenGLExtensions::instance().glMemoryBarrier(get_barriers(barriers));
    CHECK_GL_ERRORS();
}

void OpenGL3Driver::set_atomic(const Atomic& atomic, size_t unit)
{
    const OpenGL3Atomic* impl = static_cast<const OpenGL3Atomic*>(atomic.impl());
    impl->bind(unit);
}

}}
