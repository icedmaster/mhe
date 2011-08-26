#include "gui/label.hpp"

namespace mhe {
namespace gui {

	int Label::supported_handlers[Label::supported_handlers_number] = {OnMouseMove, OnMouseLeft,
																	   OnMouseLeftClick, OnMouseRightClick};

	Label::Label() :
		mouse_on_(false),
		enabled_(true)
	{
	}

	void Label::draw_impl(const boost::shared_ptr<iDriver>& driver)
	{
		Widget::draw_rect(driver, texture_);
	}

	void Label::set_handler(int event, const guieventptr& handler)
	{
		int* it = std::find(supported_handlers,
							supported_handlers + supported_handlers_number,
							event);
		if ( it == (supported_handlers + supported_handlers_number) ) return;
		// delete previous handler
		handlers.erase(event);
		// set new handler
		handlers[event] = handler;
	}

	guieventptr Label::get_gui_handler(int event) const
	{
		if (!get_visible() || !enabled_) return guieventptr();
		std::map<int, guieventptr>::const_iterator it = handlers.find(event);
		if (it == handlers.end())
			return guieventptr();
		return it->second;
	}

}
}
