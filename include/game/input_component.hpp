#ifndef __INPUT_COMPONENT_HPP__
#define __INPUT_COMPONENT_HPP__

#include "component.hpp"
#include "events/event_listener.hpp"
#include "events/keyboard_device.hpp"
#include "events/mouse_device.hpp"
#include "events/keyboard_event.hpp"
#include "events/mouse_event.hpp"
#include "math/rect.hpp"

namespace mhe {
namespace game {

class BaseInputComponent : public Component, public EventListener
{
public:
	BaseInputComponent(const std::string& name) :
		Component(name), EventListener()
	{}

	virtual ~BaseInputComponent() {}
private:
	virtual bool update_impl(const Message&)
	{
		return true;
	}

	virtual void do_subscribe(Component* /*parent*/)
	{}
};

class KeyboardInputComponent : public BaseInputComponent
{
public:
	KeyboardInputComponent(const std::string& name) :
		BaseInputComponent(name)
	{}

	virtual ~KeyboardInputComponent() {}
	EventType type() const
	{
		return keyboard_event_type;
	}

	int arg() const
	{
		return KeyboardDevice::key_up;
	}

	bool handle(const Event* event)
	{
		const KeyboardEvent* ke = static_cast<const KeyboardEvent*>(event);
		handle_key_impl(ke->sym());
		return false;
	}
private:
	virtual void handle_key_impl(int key) = 0;
};

class MouseInputComponent : public BaseInputComponent
{
public:
	MouseInputComponent(const std::string& name) :
		BaseInputComponent(name),
		mouse_on_(false)
	{}

	MouseInputComponent(const std::string& name, 
						const rect<float>& rect) :
		BaseInputComponent(name),
		rect_(rect), mouse_on_(false)
	{}

	virtual ~MouseInputComponent() {}
	EventType type() const
	{
		return mouse_event_type;
	}

	virtual int arg() const
	{
		return Event::any_event;
	}
	
	void set_geometry(const rect<float>& rect)
	{
	    rect_ = rect;
	}

	const rect<float>& geometry() const
	{
		return rect_;
	}

	bool handle(const Event* event);
private:
    virtual void handle_mouse_impl(const v2d& pos, int event_type, int button) = 0;
	virtual void handle_mouse_on();
	virtual void handle_mouse_off();
private:
	rect<float> rect_;
	bool mouse_on_;
};

}}

#endif
