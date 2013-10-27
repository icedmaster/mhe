#include "utils/graphics_utils.hpp"

#include "sprite.hpp"

namespace mhe {
namespace utils {

namespace {

const std::string sprite_shader = "diffuse_unlit";

}

Sprite* create_sprite(const std::string& filename, const Context& context)
{
	boost::shared_ptr<Texture> texture =
		context.texture_manager().get(filename);
	Sprite* sprite = new Sprite();
	sprite->set_material(material_ptr(new Material(texture, context.shader_manager().get(sprite_shader))));
	return sprite;
}

Sprite* create_sprite(const colorf& color, const mhe::vector2<float>& size, const Context& context)
{
	Sprite* sprite = new Sprite;
	sprite->set_color(color);
	sprite->set_size(size.x(), size.y());
	sprite->set_material(material_ptr(new Material(context.texture_manager().get(default_resource_name),
												   context.shader_manager().get(sprite_shader))));
	return sprite;
}

}}
