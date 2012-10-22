#ifndef __MOUSE_MESSAGES_HPP__
#define __MOUSE_MESSAGES_HPP__

#include "message.hpp"

namespace mhe {
namespace game {

class MouseOnMessage : public Message
{
public:
	MouseOnMessage(Component* sender = nullptr) :
		Message(sender)
	{}
private:
	int get_type_impl() const
	{
		return mouse_on_event;
	}
};

class MouseOffMessage : public Message
{
public:
	MouseOffMessage(Component* sender = nullptr) :
		Message(sender)
	{}
private:
	int get_type_impl() const
	{
		return mouse_left_event;
	}
};

}}

#endif
