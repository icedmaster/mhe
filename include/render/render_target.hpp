#ifndef __RENDER_TARGET_HPP__
#define __RENDER_TARGET_HPP__

#include "core/unique_ptr.hpp"
#include "core/config.hpp"
#include "utils/pool_utils.hpp"
#include "definitions.hpp"

namespace mhe {

enum
{
	rt_read,
	rt_write,
	rt_readwrite
};

struct RenderTargetDesc
{
	size_t width;
	size_t height;
	int target;
	int color_format[max_simultaneous_render_targets_number];
	int depth_format;
	int color_datatype[max_simultaneous_render_targets_number];
	int depth_datatype;
	uint8_t color_targets;
	bool use_depth;
	bool use_stencil;
};

class RenderTargetImpl
{
public:
	virtual ~RenderTargetImpl() {}
	virtual bool init(const RenderTargetDesc& desc) = 0;
	virtual void close() = 0;
};

class MHE_EXPORT RenderTarget
{
	POOL_ELEMENT_METHODS(uint16_t);
public:
	RenderTarget();

	bool init(const RenderTargetDesc& desc)
	{
		return impl_->init(desc);
	}

	void close()
	{
		impl_->close();
	}

	const RenderTargetImpl* impl() const
	{
		return impl_.get();
	}
private:
	unique_ptr<RenderTargetImpl> impl_;
};

}

#endif
