#ifndef __OPENGLES_VIDEO_DRIVER_FACTORY_HPP__
#define __OPENGLES_VIDEO_DRIVER_FACTORY_HPP__

#include "impl/abstract_video_driver_factory.hpp"
#include "core/types.hpp"
#include "opengl3_driver.hpp"
#include "opengl3_buffer.hpp"
#include "opengl3_shader_program.hpp"
#include "opengl3_texture.hpp"
#include "opengl3_render_state.hpp"
#include "opengl3_render_target.hpp"
#include "opengl3_query.hpp"
#include "opengl3_atomics.hpp"

namespace mhe {
namespace opengl {

class OpenGL3VideoDriverFactory : public AbstractVideoDriverFactory
{
public:
    const char* name() const override
    {
        return "opengl3";
    }

    DriverImpl* create_video_driver() const override
    {
        return new OpenGL3Driver;
    }

    RenderBufferImpl* create_render_buffer() const override
    {
        return new OpenGL3Buffer;
    }

    IndexBufferImpl* create_index_buffer() const override
    {
        return new OpenGL3IndexBuffer;
    }

    LayoutImpl* create_layout() const override
    {
        return new OpenGL3Layout;
    }

    UniformBufferImpl* create_uniform_buffer() const override
    {
        return new OpenGL3UniformBuffer;
    }

    ShaderProgramImpl* create_shader_program() const override
    {
        return new OpenGL3ShaderProgram;
    }

    TextureImpl* create_texture() const override
    {
        return new OpenGL3Texture;
    }

    RenderStateImpl* create_render_state() const override
    {
        return new OpenGL3RenderState;
    }

    RenderTargetImpl* create_render_target() const override
    {
        return new OpenGL3RenderTarget;
    }

    TextureBufferImpl* create_texture_buffer() const override
    {
        return new OpenGL3TextureBuffer;
    }

    ShaderStorageBufferImpl* create_shader_storage_buffer() const override
    {
        return new OpenGL3ShaderStorageBuffer;
    }

    QueryImpl* create_query() const override
    {
        return new OpenGL3Query;
    }

    AtomicImpl* create_atomic() const override
    {
        return new OpenGL3Atomic;
    }
};

}}

#endif
