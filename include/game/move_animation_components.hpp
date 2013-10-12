#ifndef __MOVE_ANIMATION_COMPONENTS_HPP__
#define __MOVE_ANIMATION_COMPONENTS_HPP__

#include "animation_component.hpp"
#include "node_component.hpp"

namespace mhe {
namespace game {

class MoveAnimationComponent : public AnimationComponent
{
public:
	MoveAnimationComponent(cmn::uint duration, const std::string& name) :
		AnimationComponent(duration, name),
		parent_node_(nullptr)
	{}

	virtual ~MoveAnimationComponent() {}
protected:
	NodeComponent* node_component() const
	{
		return parent_node_;
	}
private:
	void do_subscribe(Component* component);
private:
	NodeComponent* parent_node_;	
};

// translation
class TranslateByAnimationComponent : public MoveAnimationComponent
{
public:
	TranslateByAnimationComponent(cmn::uint duration, const std::string& name) :
		MoveAnimationComponent(duration, name)
	{}

	virtual ~TranslateByAnimationComponent() {}

	void set_translation(const vector3<float>& translation)
	{
		translation_ = translation;
	}

	void set_translation(float x, float y, float z)
	{
		translation_.set(x, y, z);
	}

	vector3<float> translation() const
	{
		return translation_;
	}
protected:
	virtual void start_impl()
	{
		initial_position_ = node_component()->node()->position();
	}

	vector3<float> initial_position() const
	{
		return initial_position_;
	}
private:
	std::string add_name_impl() const
	{
		return "translate_by";
	}

	void update_animation_impl(float v);
private:
	vector3<float> initial_position_;
	vector3<float> translation_;
};

class TranslateToAnimationComponent : public TranslateByAnimationComponent
{
public:
	TranslateToAnimationComponent(cmn::uint duration, const std::string& name) :
		TranslateByAnimationComponent(duration, name)
	{}
private:
	std::string add_name_impl() const
	{
		return "translate_to";
	}

	void start_impl()
	{
		TranslateByAnimationComponent::start_impl();
		set_translation(translation() - initial_position());
	}
};

// rotation
class RotateByAnimationComponent : public MoveAnimationComponent
{
public:
	RotateByAnimationComponent(cmn::uint duration, const std::string& name) :
		MoveAnimationComponent(duration, name)
	{}

	virtual ~RotateByAnimationComponent() {}

	void set_rotation(const vector3<float>& rotation)
	{
		rotation_ = rotation;
	}

	void set_rotation(float x, float y, float z)
	{
		rotation_.set(x, y, z);
	}

	vector3<float> rotation() const
	{
		return rotation_;
	}
protected:
	virtual void start_impl()
	{
		initial_rotation_ = node_component()->node()->rotation();
	}

	vector3<float> initial_rotation() const
	{
		return initial_rotation_;
	}
private:
	std::string add_name_impl() const
	{
		return "rotate_by";
	}

	void update_animation_impl(float v);
private:
	vector3<float> initial_rotation_;
	vector3<float> rotation_;
};

class RotateToAnimationComponent : public RotateByAnimationComponent
{
public:
	RotateToAnimationComponent(cmn::uint duration, const std::string& name) :
		RotateByAnimationComponent(duration, name)
	{}
private:
	std::string add_name_impl() const
	{
		return "rotate_to";
	}

	void start_impl()
	{
		RotateByAnimationComponent::start_impl();
		set_rotation(rotation() - initial_rotation());
	}
};

// scaling
class ScaleByAnimationComponent : public MoveAnimationComponent
{
public:
	ScaleByAnimationComponent(cmn::uint duration, const std::string& name) :
		MoveAnimationComponent(duration, name)
	{}

	virtual ~ScaleByAnimationComponent() {}

	void set_scale(const vector3<float>& scale)
	{
		scale_ = scale;
	}

	void set_scale(float x, float y, float z)
	{
		scale_.set(x, y, z);
	}

	vector3<float> scale() const
	{
		return scale_;
	}
protected:
	virtual void start_impl()
	{
		initial_scale_ = node_component()->node()->scaling();
	}

	vector3<float> initial_scale() const
	{
		return initial_scale_;
	}
private:
	std::string add_name_impl() const
	{
		return "scale_by";
	}

	void update_animation_impl(float v);
private:
	vector3<float> initial_scale_;
	vector3<float> scale_;
};

class ScaleToAnimationComponent : public ScaleByAnimationComponent
{
public:
	ScaleToAnimationComponent(cmn::uint duration, const std::string& name) :
		ScaleByAnimationComponent(duration, name)
	{}
private:
	std::string add_name_impl() const
	{
		return "scale_to";
	}

	void start_impl()
	{
		ScaleByAnimationComponent::start_impl();
		set_scale(scale() - initial_scale());
	}
};

}}

#endif
