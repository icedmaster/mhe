#ifndef __RENDER_STATE_HPP__
#define __RENDER_STATE_HPP__

#include "core/unique_ptr.hpp"
#include "utils/pool_utils.hpp"

namespace mhe {

struct DepthDesc
{
	bool enabled;

	DepthDesc() :
		enabled(true)
	{}
};

struct StencilDesc
{
	bool enabled;

	StencilDesc() :
		enabled(true)
	{}
};

enum BlendFunc
{
	blend_add
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
	BlendFunc func;
	BlendFunc func_alpha;
	BlendMode srcmode;
	BlendMode dstmode;
	BlendMode srcmode_alpha;
	BlendMode dstmode_alpha;

	BlendDesc() :
		enabled(false),
		func(blend_add), func_alpha(blend_add),
		srcmode(blend_one), dstmode(blend_zero),
		srcmode_alpha(blend_one), dstmode_alpha(blend_zero)
	{}
};

struct RenderStateDesc
{
	DepthDesc depth;
	StencilDesc stencil;
	BlendDesc blend;
};

class RenderStateImpl
{
public:
	virtual ~RenderStateImpl() {}
	virtual bool init(const RenderStateDesc& desc) = 0;
	virtual void update(const RenderStateDesc& desc) = 0;
	virtual void close() = 0;
};

class MHE_EXPORT RenderState
{
	POOL_ELEMENT_METHODS(uint16_t);
public:
	RenderState();
	~RenderState()
	{
		impl_->close();
	}

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

	const RenderStateImpl* impl() const
	{
		return impl_.get();
	}
private:
	unique_ptr<RenderStateImpl> impl_;
};

}

#endif
