#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include "core/unique_ptr.hpp"
#include "utils/pool_utils.hpp"
#include "definitions.hpp"

namespace mhe {

// texture type (target)
enum
{
    texture_2d,
    texture_cube,
    texture_3d
};

// filter type
enum
{
    texture_filter_nearest,
    texture_filter_linear,
    texture_filter_nearest_mipmap_nearest,
    texture_filter_linear_mipmap_nearest,
    texture_filter_nearest_mipmap_linear,
    texture_filter_linear_mipmap_linear
};

// texture address mode
enum
{
    texture_wrap,
    texture_clamp,
    texture_clamp_to_edge,
    texture_border
};

enum
{
    texture_posx,
    texture_negx,
    texture_posy,
    texture_negy,
    texture_posz,
    texture_negz
};

struct TextureDesc
{
    int type;
    size_t width;
    size_t height;
    size_t depth;
    int mag_filter;
    int min_filter;
    int address_mode_t;
    int address_mode_s;
    int address_mode_r;
    float anisotropic_level;
    int format;
    int datatype;
    size_t mips;

    TextureDesc() :
        type(texture_2d), mag_filter(texture_filter_linear), min_filter(texture_filter_linear),
        address_mode_t(texture_wrap), address_mode_s(texture_wrap), address_mode_r(texture_wrap), anisotropic_level(1.0f),
        format(format_rgba), datatype(format_ubyte), mips(0)
    {}
};

class TextureImpl
{
public:
    virtual ~TextureImpl() {}
    virtual bool init(const TextureDesc& desc, const uint8_t* data, size_t size) = 0;
    virtual void close() = 0;
    virtual void update(const uint8_t* data) = 0;
    virtual void copy_framebuffer() = 0;
    virtual void read(uint8_t* data, size_t size) = 0;
    virtual size_t width() const = 0;
    virtual size_t height() const = 0;
};

class MHE_EXPORT Texture
{
    POOL_ELEMENT_METHODS(uint16_t);
public:
    Texture();
    ~Texture();

    bool init(const TextureDesc& desc, const uint8_t* data, size_t size)
    {
        return impl_->init(desc, data, size);
    }

    void close()
    {
        impl_->close();
    }

    const TextureImpl* impl() const
    {
        return impl_;
    }

    void update(const uint8_t* data)
    {
        impl_->update(data);
    }

    void copy_framebuffer()
    {
        impl_->copy_framebuffer();
    }

    void read(uint8_t* data, size_t size)
    {
        impl_->read(data, size);
    }

    size_t width() const
    {
        return impl_->width();
    }

    size_t height() const
    {
        return impl_->height();
    }
private:
    TextureImpl* impl_;
};

struct TextureInstance
{
    Texture::IdType id;
    TextureInstance() : id(Texture::invalid_id) {}
};

}

#endif
