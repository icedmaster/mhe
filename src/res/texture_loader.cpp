#include "res/texture_loader.hpp"

#include <fstream>

#include "render/context.hpp"
#include "render/image.hpp"

#include "res/image_loaders/image_loaders_helper.hpp"
#include "res/image_loaders/cubemap_loader.hpp"

#include "utils/file_utils.hpp"

namespace mhe {

namespace detail {

int texture_format(int image_format)
{
	int formats[] = {image_rgba, image_bgra};
	return formats[image_format];
}

bool create_texture(Texture& texture, int target, const Image& image)
{
	TextureDesc desc;
	desc.type = target;
	desc.width = image.width;
	desc.height = image.height;
	desc.format = texture_format(image.mode);
	if (target == texture_cube)
		desc.address_mode_s = desc.address_mode_t = texture_clamp_to_edge;
	return texture.init(desc, &image.data[0], image.data.size());
}

}

bool TextureLoader::load(type& res, const std::string& name, const context_type* context)
{
	const std::string& ext = utils::get_file_extension(name);
	bool result = false;
	Image image;
	int target = texture_2d;
	if (ext == "cubemap")
	{
		std::ifstream f(name.c_str(), std::ios::in | std::ios::binary);
		if (!f.is_open()) return false;
		result = load_cubemap(image, f, name);
		target = texture_cube;
		f.close();
	}
	else result = load_image_by_extension(image, name);

	if (!result) return false;
	res.id = context->texture_pool.create();
	Texture& texture = context->texture_pool.get(res.id);
	return detail::create_texture(texture, target, image);
}

}