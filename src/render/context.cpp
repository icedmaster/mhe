#include "render/context.hpp"

namespace mhe {

RenderTarget& RenderTargetManager::create(Context& context, RenderTargetDesc& desc, float scale)
{
    desc.width = static_cast<size_t>(context.window_system.width() * scale);
    desc.height = static_cast<size_t>(context.window_system.height() * scale);
    RenderTarget& rt = create_and_get(context.render_target_pool);
    rt.init(context, desc);
    RenderTargetData rt_data;
    rt_data.desc = desc;
    rt_data.scale = scale;
    rt_data.render_target_id = rt.id();
    render_targets_.push_back(rt_data);
    return rt;
}

TextureInstance& RenderTargetManager::create(Context& context, TextureDesc& desc, float scale)
{
    desc.type = texture_2d;
    desc.address_mode_s = desc.address_mode_t = texture_clamp;
    desc.anisotropic_level = 0.0f;
    desc.mips = 0;
    desc.height = static_cast<uint>(context.window_system.height() * scale);
    desc.width = static_cast<uint>(context.window_system.width() * scale);
    Texture& texture = create_and_get(context.texture_pool);
    texture.init(desc, nullptr, 0);
    TextureData texture_data;
    texture_data.desc = desc;
    texture_data.scale = scale;
    texture_data.texture.id = texture.id();
    textures_.push_back(texture_data);
    return textures_.back().texture;
}

RenderTarget& RenderTargetManager::get_temp_render_target(Context& context, int format, float scale)
{
    for (size_t i = 0, size = temporary_render_targets_.size(); i < size; ++i)
    {
        if (temporary_render_targets_[i].scale == scale &&
            temporary_render_targets_[i].desc.color_targets == 1 &&
            temporary_render_targets_[i].desc.color_format[0] == format &&
            temporaty_render_targets_usage_.find(temporary_render_targets_[i].render_target_id) == temporaty_render_targets_usage_.end())
        {
            temporaty_render_targets_usage_[temporary_render_targets_[i].render_target_id] = true;
            return context.render_target_pool.get(temporary_render_targets_[i].render_target_id);
        }
    }

    RenderTargetDesc rt_desc;
    rt_desc.color_datatype[0] = format_default;
    rt_desc.color_format[0] = format;
    rt_desc.color_targets = 1;
    rt_desc.texture_type = texture_2d;
    rt_desc.target = rt_readwrite;
    rt_desc.use_depth = false;
    rt_desc.use_stencil = false;
    RenderTarget& render_target = create(context, rt_desc, scale);
    temporaty_render_targets_usage_[render_target.id()] = true;
    RenderTargetData& rt_data = temporary_render_targets_.add();
    rt_data.desc = rt_desc;
    rt_data.render_target_id = render_target.id();
    rt_data.scale = scale;

    return render_target;
}

void RenderTargetManager::free_temp_render_target(RenderTargetHandleType id)
{
    temporaty_render_targets_usage_.erase(id);
}

void RenderTargetManager::free_all_temp_render_targets()
{
    temporaty_render_targets_usage_.clear();
}

}
