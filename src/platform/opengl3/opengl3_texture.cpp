#include "platform/opengl3/opengl3_texture.hpp"

#include "platform/opengl/opengl_types.hpp"
#include "platform/opengl/opengl_extension.hpp"
#include "platform/opengl/opengl_helpers.hpp"

namespace mhe {
namespace opengl {

bool OpenGL3Texture::init(const TextureDesc& desc, const uint8_t* data, size_t size)
{
    glGenTextures(1, &id_);
    if (id_ == 0) return false;
    target_ = get_texture_target(desc.type);
    glBindTexture(target_, id_);
    if (target_ != GL_TEXTURE_BUFFER)
    {
        glTexParameteri(target_, GL_TEXTURE_WRAP_T, get_texture_address_mode(desc.address_mode_t));
        glTexParameteri(target_, GL_TEXTURE_WRAP_S, get_texture_address_mode(desc.address_mode_s));
        glTexParameteri(target_, GL_TEXTURE_WRAP_R, get_texture_address_mode(desc.address_mode_s));
        glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, get_texture_filter(desc.mag_filter > texture_filter_nearest ? texture_filter_linear : texture_filter_nearest));
        glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, get_texture_filter(desc.min_filter));

        CHECK_GL_ERRORS();

        if (desc.anisotropic_level > 1.0f)
            glTexParameterf(target_, GL_TEXTURE_MAX_ANISOTROPY_EXT, desc.anisotropic_level);
    }

    format_ = get_format(desc.format);
    image_format_ = get_image_format(desc.format);
    width_ = desc.width;
    height_ = desc.height;
    datatype_ = desc.datatype == format_default ? get_datatype_by_format(desc.format) : get_datatype(desc.datatype);
    pixel_data_format_ = get_pixel_data_format(desc.format);

    if (desc.type == texture_cube)
        init_cubemap(desc, data, size);
    else if (desc.type == texture_3d)
        init_3d_texture(desc, data, size);
    else if (desc.type == texture_buffer)
        init_texture_buffer(desc, data, size);
    else
    {
        glTexImage2D(target_, 0,
            format_, // internalFormat, Specifies the number of color components in the texture
            desc.width, desc.height, 0,
            // Specifies the format of the pixel data. The following symbolic values are accepted: GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, and GL_BGRA.
            pixel_data_format_,
            datatype_, data);
        CHECK_GL_ERRORS();
    }
    if (desc.mips > 0)
    {
        // TODO: need to try glTexStorage2D - we can setup the mipmaps number then
        OpenGLExtensions::instance().glGenerateMipmap(target_);
        CHECK_GL_ERRORS();
    }
    return true;
}

void OpenGL3Texture::init_cubemap(const TextureDesc& desc, const uint8_t* data, size_t size)
{
    size_t stride = desc.width * desc.height * get_bytes_per_format(get_format(desc.format));
    ASSERT(size ? size == stride * 6 : 1, "Invalid size");
    GLenum targets[6] = {GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, GL_TEXTURE_CUBE_MAP_POSITIVE_Z};
    for (int i = 0; i < 6; ++i)
        glTexImage2D(targets[i], 0,
            get_format(desc.format), desc.width, desc.height, 0,
            get_pixel_data_format(desc.format),
            desc.datatype == format_default ? get_datatype_by_format(desc.format) : get_datatype(desc.datatype),
            data ? data + i * stride : nullptr);
    CHECK_GL_ERRORS();
}

void OpenGL3Texture::init_3d_texture(const TextureDesc& desc, const uint8_t* data, size_t /*size*/)
{
    OpenGLExtensions::instance().glTexImage3D(target_, 0,
        format_, desc.width, desc.height, desc.depth,
        0, pixel_data_format_, datatype_, data);
    CHECK_GL_ERRORS();
}

void OpenGL3Texture::init_texture_buffer(const TextureDesc&, const uint8_t* data, size_t size)
{
    bool res = vbo_.init(GL_TEXTURE_BUFFER, size, data, GL_DYNAMIC_DRAW);
    ASSERT(res, "VBO initialization for a TextureBuffer failed");
    OpenGLExtensions::instance().glTexBuffer(GL_TEXTURE_BUFFER, format_, vbo_.id());
    vbo_.disable();
}

void OpenGL3Texture::close()
{
    if (format_ == GL_TEXTURE_BUFFER)
        vbo_.close();
    glDeleteTextures(1, &id_);
}

void OpenGL3Texture::enable(size_t unit) const
{
    OpenGLExtensions::instance().glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(target_, id_);
    if (target_ == GL_TEXTURE_BUFFER)
        OpenGLExtensions::instance().glTexBuffer(GL_TEXTURE_BUFFER, format_, vbo_.id());
    unit_ = unit;
}

void OpenGL3Texture::disable() const
{
    OpenGLExtensions::instance().glActiveTexture(GL_TEXTURE0 + unit_);
    glBindTexture(target_, 0);
    if (target_ == GL_TEXTURE_BUFFER)
        vbo_.disable();
}

void OpenGL3Texture::update(const uint8_t* data)
{
    if (target_ != GL_TEXTURE_BUFFER)
    {
        glBindTexture(target_, id_);
        glTexSubImage2D(target_, 0, 0, 0, width_, height_, pixel_data_format_, datatype_, data);
        CHECK_GL_ERRORS();
        glBindTexture(target_, 0);
    }
    else
        vbo_.update(vbo_.size(), 0, data);
}

void OpenGL3Texture::copy_framebuffer()
{
    ASSERT(target_ != GL_TEXTURE_BUFFER, "This method must not be called for the texture buffers");
    glBindTexture(target_, id_);
    glCopyTexSubImage2D(target_, 0, 0, 0, 0, 0, width_, height_);
    CHECK_GL_ERRORS();
    glBindTexture(target_, 0);
}

void OpenGL3Texture::read(uint8_t* data, size_t size)
{
    NOT_IMPLEMENTED_ASSERT(target_ == GL_TEXTURE_BUFFER, "Reading from such buffers");
    ASSERT(size >= width_ * height_ * get_bytes_per_format(format_), "Buffer is too small");
    glBindTexture(target_, id_);
    glGetTexImage(target_, 0, pixel_data_format_, datatype_, data);
    CHECK_GL_ERRORS();
    glBindTexture(target_, 0);
}

void OpenGL3Texture::bind(size_t unit, int access) const
{
    OpenGLExtensions::instance().glBindImageTexture(unit, id_, 0, is_layered() ? GL_TRUE : GL_FALSE,
        0, get_access(access), image_format_);
    CHECK_GL_ERRORS();
}

}}
