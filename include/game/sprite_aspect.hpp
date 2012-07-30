#ifndef __SPRITE_ASPECT_HPP__
#define __SPRITE_ASPECT_HPP__

#include "aspect.hpp"

namespace mhe {
namespace game {

class SpriteAspect : public Aspect
{
public:
	SpriteAspect(const std::string& name, const std::string& add_name,
				 const boost::shared_ptr<Sprite>& sprite,
				 const boost::shared_ptr<Scene>& scene);	
private:
	boost::shared_ptr<Sprite> sprite_;
	boost::shared_ptr<Scene> scene_;
};

}}

#endif
