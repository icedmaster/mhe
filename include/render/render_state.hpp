#ifndef __RENDER_STATE_HPP__
#define __RENDER_STATE_HPP__

#include "core/unique_ptr.hpp"
#include "utils/pool_utils.hpp"
#include "math/rect.hpp"

namespace mhe {

enum CompareMode
{
    compare_mode_always,
    compare_mode_equal,
    compare_mode_not_equal
};

struct DepthDesc
{
    bool test_enabled;
    bool write_enabled;

    DepthDesc() :
        test_enabled(true), write_enabled(true)
    {}
};

enum StencilOp
{
    stencil_op_keep,
    stencil_op_replace
};

struct StencilDesc
{
    CompareMode front_compare_func;
    StencilOp front_operation_pass;
    StencilOp front_operation_dfail;
    StencilOp front_operation_sfail;
    bool enabled;
    uint8_t front_reference;
    uint8_t front_mask;

    StencilDesc() :
        front_compare_func(compare_mode_equal),
        front_operation_pass(stencil_op_keep),
        front_operation_dfail(stencil_op_keep),
        front_operation_sfail(stencil_op_keep),
        enabled(false), front_reference(0), front_mask(0xff)
    {}
};

enum BlendFunc
{
    blend_add,
    blend_mul,
    blend_min,
    blend_max,
    blend_func_max
};

enum BlendMode
{
    blend_zero,
    blend_one,
    blend_src_alpha,
    blend_src_inv_alpha
};

struct BlendDesc
{
    bool enabled;
    bool color_write;
    BlendFunc func;
    BlendFunc func_alpha;
    BlendMode srcmode;
    BlendMode dstmode;
    BlendMode srcmode_alpha;
    BlendMode dstmode_alpha;

    BlendDesc() :
        enabled(false), color_write(true),
        func(blend_add), func_alpha(blend_add),
        srcmode(blend_one), dstmode(blend_zero),
        srcmode_alpha(blend_one), dstmode_alpha(blend_zero)
    {}
};

enum WindingOrder
{
    winding_cw,
    winding_ccw
};

enum CullMode
{
    cull_none,
    cull_front,
    cull_back,
    cull_front_and_back
};

struct RasterizerDesc
{
    CullMode cull;
    WindingOrder order;
    bool depth_test_enabled;

    RasterizerDesc() :
        cull(cull_back), order(winding_ccw), depth_test_enabled(true)
    {}
};

struct ViewportDesc
{
    rect<int> viewport;

    ViewportDesc() : viewport(0, 0, 0, 0) {}
};

struct ScissorDesc
{
    rect<int> scissor_rect;
    bool enabled;

    ScissorDesc() : scissor_rect(0, 0, 0, 0), enabled(false) {}
};

struct RenderStateDesc
{
    DepthDesc depth;
    StencilDesc stencil;
    BlendDesc blend;
    RasterizerDesc rasterizer;
    ViewportDesc viewport;
    ScissorDesc scissor;
};

class RenderStateImpl
{
public:
    virtual ~RenderStateImpl() {}
    virtual bool init(const RenderStateDesc& desc) = 0;
    virtual void update(const RenderStateDesc& desc) = 0;
    virtual void update_viewport(const ViewportDesc& viewport_desc) = 0;
    virtual void update_scissor(const ScissorDesc& scissor_desc) = 0;
    virtual void update_rasterizer(const RasterizerDesc& rasterizer_desc) = 0;
    virtual void close() = 0;
};

class MHE_EXPORT RenderState
{
    POOL_ELEMENT_METHODS(uint16_t)
public:
    RenderState();
    ~RenderState();

    bool init(const RenderStateDesc& desc)
    {
        return impl_->init(desc);
    }

    void close()
    {
        impl_->close();
    }

    void update(const RenderStateDesc& desc)
    {
        impl_->update(desc);
    }

    void update_viewport(const ViewportDesc& viewport_desc)
    {
        impl_->update_viewport(viewport_desc);
    }

    void update_scissor(const ScissorDesc& scissor_desc)
    {
        impl_->update_scissor(scissor_desc);
    }

    void update_rasterizer(const RasterizerDesc& rasterizer_desc)
    {
        impl_->update_rasterizer(rasterizer_desc);
    }

    const RenderStateImpl* impl() const
    {
        return impl_;
    }
private:
    RenderStateImpl* impl_;
};

}

#endif
