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

class MouseButtonPressMessage : public Message
{
public:
	MouseButtonPressMessage(int button, const vector2<int>& position,
							Component* sender = nullptr) :
		Message(sender), button_(button), pos_(position)
	{}

	int button() const
	{
		return button_;
	}

	const vector2<int>& position() const
	{
		return pos_;
	}
private:
	int get_type_impl() const
	{
		return mouse_click_event;
	}

	int button_;
	vector2<int> pos_;
};

}}

#endif
