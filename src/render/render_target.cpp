#include "render/render_target.hpp"

#include "impl/system_factory.hpp"
#include "render/context.hpp"

namespace mhe {

RenderTarget::RenderTarget() :
	impl_(SystemFactory::instance().create_render_target())
{}

bool RenderTarget::init(Context& context, const RenderTargetDesc& desc)
{
	desc_ = desc;
	Texture* rt[max_simultaneous_render_targets_number];
	for (size_t i = 0; i < desc.color_targets; ++i)
	{
		rt[i] = &create_and_get(context.texture_pool);
		rt_[i].id = rt[i]->id();
	}

	Texture* ds = nullptr;
	if (desc.use_depth || desc.use_stencil)
	{
		ds_.id = context.texture_pool.create();
		ds = &context.texture_pool.get(ds_.id);
	}

	return impl_->init(desc, rt, ds);
}

size_t RenderTarget::color_textures(const TextureInstance** ids) const
{
	*ids = rt_;
	return desc_.color_targets;
}

size_t RenderTarget::depth_texture(TextureInstance& id) const
{
	id = ds_;
	return (desc_.use_depth || desc_.use_stencil) ? 1 : 0;
}

}
