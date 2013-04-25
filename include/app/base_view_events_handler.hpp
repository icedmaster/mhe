#ifndef __BASE_VIEW_EVENTS_HANDLER_HPP__
#define __BASE_VIEW_EVENTS_HANDLER_HPP__

#include "view.hpp"

namespace mhe {
namespace app {

class BaseViewEventsHandler : public ViewEventsHandler
{
public:
	BaseViewEventsHandler(game::Engine* engine) :
		engine_(engine)
	{}

	void on_draw()
	{
		engine_->render();
	}
private:
	game::Engine* engine_;
};

}}

#endif
