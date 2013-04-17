#ifndef __VIEW_HPP__
#define __VIEW_HPP__

#include <boost/scoped_ptr.hpp>
#include "types.hpp"
#include "math/vector2.hpp"

namespace mhe {

class ViewEventsHandler
{
public:
	virtual ~ViewEventsHandler() {}

	virtual void on_draw() = 0;
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
protected:
	void on_draw()
	{
		events_handler_->on_draw();
	}

	ViewEventsHandler* events_handler()
	{
		return events_handler_.get();
	}
private:
	virtual void update() {}

	boost::scoped_ptr<ViewEventsHandler> events_handler_;
};

}

#endif
