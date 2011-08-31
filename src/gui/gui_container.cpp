#include "gui/gui_container.hpp"

namespace mhe {
namespace gui {

	GUIContainer::GUIContainer() {}

	GUIContainer::GUIContainer(InputSystem& is)
	{
        setupEvents(is);
	}

	void GUIContainer::add(const boost::shared_ptr<Widget>& widget)
	{
		widgets_.push_back(widget);
	}

	boost::shared_ptr<Widget> GUIContainer::get(const std::string& name) const
	{
		for (size_t i = 0; i < widgets_.size(); ++i)
		{
			if (widgets_[i]->getName() == name)
				return widgets_[i];
		}
		return boost::shared_ptr<Widget>();
	}

	void GUIContainer::draw_impl(const boost::shared_ptr<iDriver>& driver)
	{
		for (size_t i = 0; i < widgets_.size(); ++i)
			widgets_[i]->draw(driver);
	}

	bool GUIContainer::handle_mouse_click(const Event& e)
	{
		const MouseEvent& me = dynamic_cast<const MouseEvent&>(e);

		for (size_t i = 0; i < widgets_.size(); ++i)
		{
			if (widgets_[i]->handleMouseClick(me))
				return true;
		}

		return true;
	}

	bool GUIContainer::handle_mouse_move(const Event& e)
	{
		const MouseEvent& me = dynamic_cast<const MouseEvent&>(e);

		for (size_t i = 0; i < widgets_.size(); ++i)
		{
			if (widgets_[i]->handleMouseMove(me))
				return true;
		}

		return true;
	}

	bool GUIContainer::handle_keyboard(const Event&)
	{
		return true;
	}

	void GUIContainer::setupEvents(InputSystem& is)
	{
        is.addListener(new gui_event_handler(MouseEventType, MOUSE_BUTTON_PRESSED, 0,
											 this,
                                             &GUIContainer::handle_mouse_click));
		is.addListener(new gui_event_handler(MouseEventType, MOUSE_BUTTON_RELEASED, 0,
											 this,
                                             &GUIContainer::handle_mouse_click));
		is.addListener(new gui_event_handler(MouseEventType, MOUSE_MOVE, 0,
											 this,
                                             &GUIContainer::handle_mouse_move));
	}

}
}
