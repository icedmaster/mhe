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
	move,
	wheel
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
		pos_(pos),
        buttons_(button)
	{}

	MouseEvent() :
		Event(mouse_event_type)
	{}

	void setup_event(int event, int button, const vector2<int>& pos, int buttons_mask)
	{
		Event::setup_event(event, button);
		pos_ = pos;
		buttons_ = buttons_mask;
	}

	void setup_wheel_event(float xdelta, float ydelta, int buttons_mask)
	{
		Event::setup_event(wheel);
		wheel_delta_.set(xdelta, ydelta);
		buttons_ = buttons_mask;
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

	bool left_button_pressed() const
	{
		return buttons_ & left_button;
	}

	bool right_button_pressed() const
	{
		return (buttons_ & right_button) > 0;
	}

	bool any_button_pressed() const
	{
		return buttons_ > 0;
	}

    void invert_y_position(int height = 0)
	{
		pos_.set_y(height - pos_.y());
	}

	vector2<float> wheel_delta() const
	{
		return wheel_delta_;
	}
private:
	vector2<int> pos_;
	vector2<float> wheel_delta_;
	int buttons_;
};

}

#endif
