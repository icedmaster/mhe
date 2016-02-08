#ifndef __RENDER_TARGET_HPP__
#define __RENDER_TARGET_HPP__

#include "core/unique_ptr.hpp"
#include "core/config.hpp"
#include "utils/pool_utils.hpp"
#include "definitions.hpp"
#include "texture.hpp"

namespace mhe {

struct Context;

enum
{
    rt_read,
    rt_write,
    rt_readwrite
};

struct RenderTargetDesc
{
    // TODO: make different widths and heights for each render target
    size_t width;
    size_t height;
    int target;
    int texture_type;
    int color_format[max_simultaneous_render_targets_number];
    int depth_format;
    int color_datatype[max_simultaneous_render_targets_number];
    int depth_datatype;
    int color_filter[max_simultaneous_render_targets_number];
    int depth_filter;
    int color_address_mode[max_simultaneous_render_targets_number];
    float color_anisotropy[max_simultaneous_render_targets_number];
    float depth_anisotropy;
    uint8_t color_targets;
    bool use_depth;
    bool use_stencil;

    RenderTargetDesc() : texture_type(texture_2d)
    {
        for (size_t i = 0; i < max_simultaneous_render_targets_number; ++i)
        {
            color_filter[i] = texture_filter_linear;
            color_anisotropy[i] = 1.0f;
            color_address_mode[i] = texture_clamp;
        }
        depth_filter = texture_filter_linear;
        depth_anisotropy = 1.0f;
    }
};

class RenderTargetImpl
{
public:
    virtual ~RenderTargetImpl() {}
    virtual bool init(const RenderTargetDesc& desc, Texture** color_textures, Texture* depth_texture) = 0;
    virtual void close() = 0;
    virtual void set_texture_side(int side) = 0;
};

class MHE_EXPORT RenderTarget
{
    POOL_ELEMENT_METHODS(uint16_t)
public:
    RenderTarget();
    ~RenderTarget();

    bool init(Context& context, const RenderTargetDesc& desc);

    void close();

    const RenderTargetImpl* impl() const
    {
        return impl_;
    }

    size_t color_textures(const TextureInstance** ids) const;
    size_t color_textures(TextureInstance* ids) const;
    size_t depth_texture(TextureInstance& id) const;
    size_t color_texture(TextureInstance& id, size_t index) const;

    size_t width() const
    {
        return desc_.width;
    }

    size_t height() const
    {
        return desc_.height;
    }

    // For cubemaps only!
    void set_texture_side(int side);
private:
    Context* context_; // it shouldn't change
    RenderTargetDesc desc_;
    TextureInstance rt_[max_simultaneous_render_targets_number];
    TextureInstance ds_;
    RenderTargetImpl* impl_;
};

static const RenderTarget::IdType default_render_target = RenderTarget::invalid_id - 1;

}

#endif
