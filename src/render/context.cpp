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

}
