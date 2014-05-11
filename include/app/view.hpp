#ifndef __VIEW_HPP__
#define __VIEW_HPP__

#include "core/types.hpp"
#include "core/unique_ptr.hpp"
#include "math/vector2.hpp"

namespace mhe {

class ViewEventsHandler
{
public:
	virtual ~ViewEventsHandler() {}
	virtual void on_draw() = 0;
	virtual void on_update() = 0;
	virtual void on_resize(unsigned int new_width, unsigned int new_height) = 0;
};

class View
{
public:
	View()	{}

	View(ViewEventsHandler* events_handler) :
		events_handler_(events_handler)
	{}
    
    virtual ~View() {}

	void set_events_handler(ViewEventsHandler* events_handler)
	{
		events_handler_.reset(events_handler);
		update();
	}

	ViewEventsHandler* events_handler()
	{
		return events_handler_.get();
	}
protected:
	void on_draw()
	{
		if (events_handler_ != nullptr)
			events_handler_->on_draw();
	}
private:
	virtual void update() {}

	unique_ptr<ViewEventsHandler> events_handler_;
};

}

#endif
