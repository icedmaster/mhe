#ifndef __OPENGL3_DRIVER_HPP__
#define __OPENGL3_DRIVER_HPP__

#include "render/video_driver.hpp"
#include "core/array.hpp"
#include "core/types.hpp"
#include "../opengl/mhe_gl.hpp"

namespace mhe {
namespace opengl {

class OpenGL3IndexBuffer;
class OpenGL3ShaderProgram;
class OpenGL3RenderTarget;

struct OpenGL3ContextState
{
    array<size_t, 16> uniforms;
    vector2<int> window_size;
    rect<int> viewport;
    rect<int> scissor;
    GLenum cull_mode;
    GLenum cull_winding;
    bool depth_test;
    bool depth_write;
    bool stencil;
    bool blend;
    bool scissor_test;
    bool cull;
    bool rasterizer_depth_test_disabled;
    uint8_t color_mask;

    OpenGL3ContextState();
};

class OpenGL3Driver : public DriverImpl
{
public:
    OpenGL3Driver();
private:
    bool init(DriverRenderingCapabilities& caps);
    void close();

    void enable_blending();
    void disable_blending();

    void enable_depth() {}
    void disable_depth() {}

    void clear_depth();
    void clear_color();
    void clear_stencil();

    void set_clear_color(const colorf&);

    void set_window_size(const vector2<int>& size)
    {
        state_.window_size = size;
    }

    void set_viewport(int x, int y, int w, int h);

    uint major_version_need() const override
    {
        return 3;
    }

    uint minor_version_need() const override
    {
        return 3;
    }

    uint supported_versions(pair<uint, uint>* versions, uint size) const override;

    void set_state(const RenderState& state);
    void set_shader_program(const ShaderProgram& program);
    void set_vertex_buffer(const RenderBuffer& vbuffer);
    void set_index_buffer(const IndexBuffer& ibuffer);
    void set_uniform(const UniformBuffer& uniform, size_t unit);
    void bind_uniform(const UniformBuffer& uniform, size_t unit) override;
    void set_layout(const Layout& layout);
    void set_texture(const Texture& texture, size_t unit);
    void set_render_target(const RenderTarget& render_target);
    void set_texture_buffer(const TextureBuffer& texture_buffer, size_t unit) override;
    void set_default_render_target();
    void set_atomic(const Atomic& atomic, size_t unit) override;
    void draw(const RenderData& data);
    void draw(size_t elements_number, size_t vbuffer_offset, size_t ibuffer_offset, size_t indices_number,
              Primitive primitive, size_t instances_count) override;

    void flush();

    void set_image(const Texture& texture, size_t unit, int access) override;
    void set_shader_storage_buffer(const ShaderStorageBuffer& buffer, size_t unit) override;
    void dispatch(size_t x, size_t y, size_t z) override;
    void memory_barrier(uint32_t barriers) override;
private:
    void setup_caps(DriverRenderingCapabilities& caps);

    pair<uint, uint> versions_[4];
    OpenGL3ContextState state_;
    const OpenGL3IndexBuffer* current_index_buffer_;
    const OpenGL3ShaderProgram* current_shader_program_;
    const OpenGL3RenderTarget* current_render_target_;
};

}}

#endif
