#ifndef __MOUSE_EVENT_HPP__
#define __MOUSE_EVENT_HPP__

#include "event.hpp"
#include "math/vector2.hpp"

namespace mhe {

class MouseEvent : public Event
{
public:
enum
{
	button_pressed = 1,
	button_released,
	move
};

// mouse buttons
enum
{
	left_button = 1,
	right_button
};

public:
	MouseEvent(int event, int button, const vector2<int>& pos) :
		Event(mouse_event_type, event, button),
		pos_(pos)
	{}

	MouseEvent() :
		Event(mouse_event_type)
	{}

	void setup_event(int event, int button, const vector2<int>& pos)
	{
		Event::setup_event(event, button);
		pos_ = pos;
	}

	const vector2<int> pos() const
	{
		return pos_;
	}

	int event_type() const
	{
		return Event::arg();
	}

	int button() const
	{
		return Event::optarg();
	}

    void invert_y_position(int height = 0)
	{
		pos_.set_y(height - pos_.y());
	}
private:
	vector2<int> pos_;
};

}

#endif
