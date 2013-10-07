#ifndef __MESSAGE_COLOR_HPP__
#define __MESSAGE_COLOR_HPP__

#include "message.hpp"

namespace mhe {
namespace game {

class ColorMessage : public Message
{
public:
	ColorMessage(Component* sender = nullptr) :
	Message(sender)
	{}

	ColorMessage(const colorf& color, Component* sender = nullptr) :
		Message(sender), color_(color)
	{}

	void set_color(const colorf& color)
	{
		color_ = color;
	}

	colorf color() const
	{
		return color_;
	}
private:
	int get_type_impl() const
	{
		return color_event;
	}

	colorf color_;
};

}}

#endif
