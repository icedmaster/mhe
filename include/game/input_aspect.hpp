#ifndef __INPUT_ASPECT_HPP__
#define __INPUT_ASPECT_HPP__

#include "aspect.hpp"
#include "events/event_listener.hpp"

namespace mhe {
namespace game {

class BaseInputAspect : public Aspect, public EventListener
{
public:
	virtual ~BaseInputAspect() {}
private:
	bool update_impl(const Message&)
	{
		return true;
	}

	void do_subscribe(Aspect* parent)
	{}
};

class KeyboardInputAspect : public BaseInputAspect
{
public:
	virtual ~KeyboardInputAspect() {}
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

class MouseInputAspect : public BaseInputAspect
{
public:
	virtual ~MouseInputAspect() {}
	EventType type() const
	{
		return mouse_event_type;
	}

	int arg() const
	{
		return Event::any_event;
	}

	bool handle(const Event* event)
	{
	    const MouseEvent* me = static_cast<const MouseEvent*>(event);
	    handle_mouse_impl(me->pos(), me->event_type(), me->button());
	    return false;
	}
private:
    virtual void handle_mouse_impl(const v2d& pos, int event_type, int button) = 0;
};

}}

#endif
