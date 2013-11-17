#include "game/frame_animation_component.hpp"

#include "assertions.hpp"
#include "game/engine.hpp"
#include "utils/file_utils.hpp"

namespace mhe {
namespace game {

FrameAnimationComponent::FrameAnimationComponent(Engine* engine, const std::string& shader_name,
												 const std::string& texture_name, cmn::uint textures_number,
												 cmn::uint duration, const std::string& name) :
	AnimationComponent(duration, name),
	node_(nullptr), frame_duration_(1 / static_cast<float>(textures_number))
{
	init(engine, shader_name, texture_name, textures_number);
}

void FrameAnimationComponent::init(Engine* engine, const std::string& shader_name,
								   const std::string& texture_name, cmn::uint textures_number)
{
	const std::string& name = utils::get_file_name(texture_name);
	const std::string& ext = utils::get_file_extension(texture_name);
	for (size_t i = 0; i < textures_number; ++i)
	{
		materials_.push_back(
			engine->context().material_manager().get_material(name + utils::types_cast<std::string>(i) + "." + ext,
															  shader_name));
	}
}

void FrameAnimationComponent::set_blend_mode(BlendMode blend_mode)
{
	for (size_t i = 0; i < materials_.size(); ++i)
		materials_[i]->set_blend_mode(blend_mode);
}

void FrameAnimationComponent::do_subscribe(Component* parent)
{
	node_ = checked_static_cast<NodeComponent*>(parent);
}

void FrameAnimationComponent::update_animation_impl(float value)
{
	size_t frame = value / frame_duration_;
	node_->node()->set_material(materials_[frame]);
}

}}
