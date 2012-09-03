#ifndef __SPRITE_ASPECT_HPP__
#define __SPRITE_ASPECT_HPP__

#include "aspect.hpp"
#include "sprite.hpp"
#include "scene.hpp"

namespace mhe {
namespace game {

class SpriteAspect : public Aspect
{
	friend class AnimationListBase;
public:
	SpriteAspect(const std::string& name, const std::string& add_name,
				 const boost::shared_ptr<Sprite>& sprite,
				 const boost::shared_ptr<Scene>& scene);
	~SpriteAspect();	
private:
	void do_subscribe(Aspect* aspect);
   	bool update_impl(const Message& message);
	void destroy_impl();
private:
	void on_animation_completed(AnimationListBase* animation_list,
								AnimationListBase::AnimationChangeType change_type);

	boost::shared_ptr<Sprite> sprite_;
	boost::shared_ptr<Scene> scene_;
};

}}

#endif
