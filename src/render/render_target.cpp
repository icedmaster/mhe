#include "render/render_target.hpp"

#include "impl/system_factory.hpp"
#include "render/context.hpp"

namespace mhe {

RenderTarget::RenderTarget() :
    impl_(SystemFactory::instance().create_render_target())
{}

RenderTarget::~RenderTarget()
{
    SystemFactory::instance().destroy_object(impl_);
}

bool RenderTarget::init(Context& context, const RenderTargetDesc& desc)
{
    context_ = &context;
    desc_ = desc;
    if (desc.texture_type == texture_cube && desc.color_targets != 1)
    {
        ASSERT(0, "Invalid render target description");
        return false;
    }
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

void RenderTarget::close()
{
    if (is_handle_valid(ds_.id))
        destroy_pool_object(context_->texture_pool, ds_.id);
    for (size_t i = 0, targets_number = desc_.color_targets; i < targets_number; ++i)
        destroy_pool_object(context_->texture_pool, rt_[i].id);
    impl_->close();
}

size_t RenderTarget::color_textures(const TextureInstance** ids) const
{
    *ids = rt_;
    return desc_.color_targets;
}

size_t RenderTarget::color_textures(TextureInstance* ids) const
{
    for (size_t i = 0; i < desc_.color_targets; ++i)
        ids[i] = rt_[i];
    return desc_.color_targets;
}

size_t RenderTarget::depth_texture(TextureInstance& id) const
{
    id = ds_;
    return (desc_.use_depth || desc_.use_stencil) ? 1 : 0;
}

size_t RenderTarget::color_texture(TextureInstance& id, size_t index) const
{
    ASSERT(index < max_simultaneous_render_targets_number, "Invalid index");
    id = rt_[index];
    return desc_.color_targets >= index ? 1 : 0;
}

void RenderTarget::set_texture_side(int side)
{
    ASSERT(desc_.texture_type == texture_cube, "Invalid call");
    impl_->set_texture_side(side);
}

}
