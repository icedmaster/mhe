#ifndef __BASE_VIEW_EVENTS_HANDLER_HPP__
#define __BASE_VIEW_EVENTS_HANDLER_HPP__

#include "engine.hpp"

namespace mhe {
namespace game {

class BaseViewEventsHandler : public ViewEventsHandler
{
public:
	BaseViewEventsHandler(Engine* engine) :
		engine_(engine)
	{}

	void on_update()
	{
		engine_->update();
	}

	void on_draw()
	{
		engine_->render();
	}

	void on_resize(unsigned int new_width, unsigned int new_height)
	{
        UNUSED(new_width);
        UNUSED(new_height);
        NOT_IMPLEMENTED_ASSERT(0, "Window resizing");
	}
private:
	Engine* engine_;
};

}}

#endif
