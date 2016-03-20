#ifndef __OPENGL3_TEXTURE_HPP__
#define __OPENGL3_TEXTURE_HPP__

#include "render/texture.hpp"
#include "../opengl/mhe_gl.hpp"

namespace mhe {
namespace opengl {

class OpenGL3Texture : public TextureImpl
{
public:
    bool init(const TextureDesc& desc, const uint8_t* data, size_t size);
    void close();

    void enable(size_t unit) const;
    void disable() const;

    GLuint id() const
    {
        return id_;
    }

    void update(const uint8_t* data) override;

    void copy_framebuffer() override;
    void read(uint8_t* data, size_t size) override;

    GLenum format() const
    {
        return format_;
    }

    GLenum image_format() const
    {
        return image_format_;
    }

    size_t width() const override
    {
        return width_;
    }

    size_t height() const override
    {
        return height_;
    }
private:
    void init_cubemap(const TextureDesc& desc, const uint8_t* data, size_t size);
    void init_3d_texture(const TextureDesc& desc, const uint8_t* data, size_t size);

    GLuint id_;
    GLenum target_;
    GLsizei width_;
    GLsizei height_;
    GLenum format_;
    GLenum image_format_;
    GLenum pixel_data_format_;
    GLenum datatype_;
    mutable size_t unit_;
};

}}

#endif
