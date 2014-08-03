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

struct BlendDesc
{
	bool enabled;

	BlendDesc() :
		enabled(false)
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

	const RenderStateImpl* impl() const
	{
		return impl_.get();
	}
private:
	unique_ptr<RenderStateImpl> impl_;
};

}

#endif
