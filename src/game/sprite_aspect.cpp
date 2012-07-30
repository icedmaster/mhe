#include "game/sprite_aspect.hpp"

namespace mhe {
namespace game {

SpriteAspect::SpriteAspect(const std::string& name, const std::string& add_name,
						   const boost::shared_ptr<Sprite>& sprite,
						   const boost::shared_ptr<Scene>& scene) :
	Aspect(name, add_name),
	sprite_(sprite), scene_(scene)
{}

}}
