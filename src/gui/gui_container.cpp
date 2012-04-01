#include "gui/gui_container.hpp"

namespace mhe {
namespace gui {

	GUIContainer::GUIContainer() :
		mouse_move_listener_(new gui_event_handler(MouseEventType, MOUSE_MOVE, 0,
												   this,
	                                               &GUIContainer::handle_mouse_move)),
		mouse_click_listener_(new gui_event_handler(MouseEventType, MOUSE_BUTTON_PRESSED, 0,
											 		this,
		                                            &GUIContainer::handle_mouse_click)),
		mouse_release_listener_(new gui_event_handler(MouseEventType, MOUSE_BUTTON_RELEASED, 0,
											 		  this,
		                                              &GUIContainer::handle_mouse_click))
	{}


	GUIContainer::GUIContainer(boost::shared_ptr<InputSystem> is) :
		mouse_move_listener_(new gui_event_handler(MouseEventType, MOUSE_MOVE, 0,
												   this,
	                                               &GUIContainer::handle_mouse_move)),
		mouse_click_listener_(new gui_event_handler(MouseEventType, MOUSE_BUTTON_PRESSED, 0,
											 		this,
		                                            &GUIContainer::handle_mouse_click)),
		mouse_release_listener_(new gui_event_handler(MouseEventType, MOUSE_BUTTON_RELEASED, 0,
											 		  this,
		                                              &GUIContainer::handle_mouse_click))
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
			if (widgets_[i]->get_name() == name)
				return widgets_[i];
		}
		return boost::shared_ptr<Widget>();
	}

	void GUIContainer::draw_impl(boost::shared_ptr<Driver> driver)
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

	void GUIContainer::setupEvents(boost::shared_ptr<InputSystem> is)
	{
        is->add_listener(mouse_move_listener_);
		is->add_listener(mouse_click_listener_);
		is->add_listener(mouse_release_listener_);
	}

}
}
