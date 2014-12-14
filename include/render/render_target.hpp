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
	virtual bool init(const RenderTargetDesc& desc, Texture** color_textures, Texture* depth_texture) = 0;
	virtual void close() = 0;
};

class MHE_EXPORT RenderTarget
{
	POOL_ELEMENT_METHODS(uint16_t);
public:
	RenderTarget();

	bool init(Context& context, const RenderTargetDesc& desc);

	void close()
	{
		impl_->close();
	}

	const RenderTargetImpl* impl() const
	{
		return impl_.get();
	}

	size_t color_textures(const TextureInstance** ids) const;
	size_t depth_texture(TextureInstance& id) const;
private:
	RenderTargetDesc desc_;
	TextureInstance rt_[max_simultaneous_render_targets_number];
	TextureInstance ds_;
	unique_ptr<RenderTargetImpl> impl_;
};

static const RenderTarget::IdType default_render_target = RenderTarget::invalid_id - 1;

}

#endif
