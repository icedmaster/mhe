#include "utils/graphics_utils.hpp"

#include "sprite.hpp"
#include "texture_animation.hpp"

namespace mhe {
namespace utils {

Sprite* create_sprite(const std::string& filename, const Context& context)
{
	boost::shared_ptr<Texture> texture =
		context.texture_manager().get(filename);
	Sprite* sprite = new Sprite();
	sprite->set_texture(texture);
	return sprite;
}

Sprite* create_sprite(const colorf& color, const mhe::vector2<float>& size, const Context& context)
{
	Sprite* sprite = new Sprite;
	sprite->set_color(color);
	sprite->set_size(size.x(), size.y());
    sprite->set_texture(boost::shared_ptr<Texture>(context.texture_manager().get(default_resource_name)));
	return sprite;
}

}}
