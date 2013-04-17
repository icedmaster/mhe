#include "utils/graphics_utils.hpp"

#include "sprite.hpp"
#include "animation_list.hpp"
#include "texture_animation.hpp"

namespace mhe {
namespace utils {

Sprite* create_sprite(const std::string& filename, const Context& context)
{
	AnimationList* al = new AnimationList(0, true);
	boost::shared_ptr<Texture> texture =
		context.texture_manager().get(filename);
	Animation* a = new TextureAnimation(1000, texture);
	al->add(a);
	return new mhe::Sprite(al);
}

Sprite* create_sprite(const colorf& color, const mhe::vector2<float>& size)
{
	Sprite* sprite = new Sprite;
	sprite->set_color(color);
	sprite->set_size(size.x(), size.y());
	return sprite;
}

}}
