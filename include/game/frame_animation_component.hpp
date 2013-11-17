#ifndef __FRAME_ANIMATION_COMPONENT_HPP__
#define __FRAME_ANIMATION_COMPONENT_HPP__

#include "animation_component.hpp"
#include "node_component.hpp"

namespace mhe {
namespace game {

class Engine;

class FrameAnimationComponent : public AnimationComponent
{
public:
	FrameAnimationComponent(Engine* engine, const std::string& shader_name,
							const std::string& texture_name, cmn::uint textures_number,
							cmn::uint duration, const std::string& name);

	void set_blend_mode(BlendMode blend_mode);
private:
	void do_subscribe(Component* parent);
	void update_animation_impl(float value);

	std::string add_name_impl() const
	{
		return "frame_animation";
	}
private:
	void init(Engine* engine, const std::string& shader_name,
			  const std::string& texture_name, cmn::uint textures_number);

	fixed_size_vector<material_ptr, 12> materials_;
	NodeComponent* node_;
	float frame_duration_;
};

}}

#endif
