#ifndef __OPENGL3_BUFFER_HPP__
#define __OPENGL3_BUFFER_HPP__

#include "core/vector.hpp"
#include "render/render_buffer.hpp"
#include "../opengl/mhe_gl.hpp"

namespace mhe {
namespace opengl {

class OpenGL3ShaderProgram;

class VAO
{
public:
    bool init();
    void close();
    void enable() const;
    void disable() const;

    GLuint id() const
    {
        return id_;
    }

    VAO& operator= (const VAO& vao)
    {
        id_ = vao.id_;
        return *this;
    }
private:
    GLuint id_;
};

class VBO
{
public:
    bool init(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
    void close();
    void enable() const;
    void disable() const;

    GLuint id() const
    {
        return id_;
    }

    void update(GLsizeiptr size, GLintptr offset, const GLvoid* data);
    void* map(GLsizeiptr size, GLintptr offset);
    void unmap();

    void data(uint8_t* dst, size_t size) const;

    size_t size() const
    {
        return size_;
    }
private:
    size_t size_;
    GLuint id_;
    GLenum target_;
    GLenum usage_;
};

class OpenGL3Buffer : public RenderBufferImpl
{
public:
    bool init(BufferUpdateType type, const uint8_t* data, size_t size, size_t element_size);
    void close();

    void update(const uint8_t* data, size_t size) override;
    void* map(size_t size, size_t offset) override;
    void unmap() override;

    void data(uint8_t* vertices, size_t size) const override;
    size_t size() const override
    {
        return vbo_.size();
    }

    const VAO& vao() const
    {
        return vao_;
    }

    const VBO& vbo() const
    {
        return vbo_;
    }

    void enable() const;
    void disable() const;
private:
    VAO vao_;
    VBO vbo_;
};

class OpenGL3IndexBuffer : public IndexBufferImpl
{
public:
    bool init(const RenderBuffer& render_buffer, const uint32_t* indexes, size_t size) override;
    bool init(BufferUpdateType type, const RenderBuffer& render_buffer, const uint32_t* indices, size_t size) override;
    void close();

    void update(const uint32_t* indices, size_t size) override;
    
    const uint32_t* get() const
    {
        return indices_.data();
    }

    size_t size() const override
    {
        return indices_.size();
    }

    void data(uint32_t* indices, size_t size) const override;

    void enable() const;
private:
    vector<uint32_t> indices_;
    VAO vao_;
    VBO vbo_;
};

class OpenGL3Layout : public LayoutImpl
{
public:
    bool init(const LayoutDesc& desc);
    void close() {}

    void enable() const;
    void disable() const;
private:
    LayoutDesc desc_;
};

class OpenGL3UniformBuffer : public UniformBufferImpl
{
#ifdef MHE_OPENGL_UBO_DOUBLEBUFFERING
    static const size_t max_vbo_index = 2;
#else
    static const size_t max_vbo_index = 1;
#endif
public:
    bool init(const UniformBufferDesc& desc);
    void close();

    void update(const UniformBufferDesc& desc);
    void update(const uint8_t* data, size_t offset, size_t size);

    void bind(size_t unit) const;
    void bind(const OpenGL3ShaderProgram* program, size_t unit) const;
    void enable(const OpenGL3ShaderProgram* program, size_t unit) const;
    void disable() const;
private:
    fixed_size_vector<uint8_t, max_uniforms_per_block * 4 * sizeof(float)> data_;
    VBO vbo_[max_vbo_index];
    GLint offsets_[max_uniforms_per_block];
    GLuint id_;
    uint8_t current_;
};

class OpenGL3TextureBuffer : public TextureBufferImpl
{
public:
    bool init(const TextureBufferDesc& desc, size_t size, const uint8_t* data) override;
    void destroy() override;
    void update(const uint8_t* data) override;

    void enable(const OpenGL3ShaderProgram* shader_program, size_t unit) const;
    void disable() const;

    void data(uint8_t* ptr, size_t size) const override;
    size_t size() const override
    {
        return vbo_.size();
    }
private:
    VBO vbo_;
    GLuint tex_id_;
    GLenum format_;
    size_t size_;
    size_t unit_;
};

class OpenGL3ShaderStorageBuffer : public ShaderStorageBufferImpl
{
public:
    bool init(const ShaderStorageBufferDesc& desc, const uint8_t* data, size_t size) override;
    void destroy() override;

    void enable(size_t unit) const;
    void disable() const;

    size_t size() const override
    {
        return vbo_.size();
    }

    void data(uint8_t* ptr, size_t size) const override
    {
        vbo_.data(ptr, size);
    }

    void update(const uint8_t* data, size_t size) override
    {
        vbo_.update(size, 0, data);
    }
private:
    VBO vbo_;
    GLenum format_;
};

}}

#endif
