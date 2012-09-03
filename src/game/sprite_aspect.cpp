#include "game/sprite_aspect.hpp"
#include "game/message_transform.hpp"

namespace mhe {
namespace game {

SpriteAspect::SpriteAspect(const std::string& name, const std::string& add_name,
						   const boost::shared_ptr<Sprite>& sprite,
						   const boost::shared_ptr<Scene>& scene) :
	Aspect(name, add_name),
	sprite_(sprite), scene_(scene)
{
	scene_->add(sprite_);
	sprite_->start();
	sprite_->current_animation_list()->add_delegate(
		create_delegate(this, &SpriteAspect::on_animation_completed));
}

SpriteAspect::~SpriteAspect()
{
	scene_->remove(sprite_);
}

void SpriteAspect::do_subscribe(Aspect* aspect)
{
	aspect->subscribe(transform_event, this);
}

bool SpriteAspect::update_impl(const Message& message)
{
	if (message.type() == transform_event)
	{
		const TransformMessage& tr_message = static_cast<const TransformMessage&>(message);
		sprite_->set_transform(tr_message.get_transform());
		update_children(message);
	}
	return true;
}

void SpriteAspect::on_animation_completed(AnimationListBase* animation_list,
										  AnimationListBase::AnimationChangeType change_type)
{
}

}}
