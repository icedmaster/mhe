#ifndef __EVENT_WIDGET_HPP__
#define __EVENT_WIDGET_HPP__

#include "gui_event_type.hpp"
#include "delegate.hpp"
#include "mhe_math.hpp"
#include "mhe_event.hpp"

namespace mhe {
namespace gui {

class EventWidget
{
	typedef Delegate1<void, EventWidget*> Delegate;
public:
	typedef DelegateFunc1<void, EventWidget*> EventHandler;
public:
	EventWidget();

	virtual ~EventWidget() {}

	void set_geom(const rect<int>& geom)
	{
		geom_ = geom;
	}

	const rect<int>& geom() const
	{
		return geom_;
	}

	bool mouse_on() const
	{
		return mouse_on_;
	}

	void add_handler(int event, EventHandler* handler);

	void process_mouse_move_event(const MouseEvent* event);
	void process_mouse_press_event(const MouseEvent* event);
	void process_mouse_release_event(const MouseEvent* event);
private:
	virtual void process_event_impl(int /*event*/, const MouseEvent*) {}
private:
	void process_event(int event, const MouseEvent* mouse_event);

	rect<int> geom_;
	bool mouse_on_;
	bool button_pressed_;
	std::map<int, Delegate> handlers_;
};

}}

#endif
