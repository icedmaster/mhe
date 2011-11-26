#ifndef __GUI_ASPECT_HPP__
#define __GUI_ASPECT_HPP__

#include "aspect.hpp"

namespace mhe {
namespace game {

class MouseInputAspect : public Aspect
{
typedef mhe::PrivateEventListener<MouseInputAspect> gui_listener;
friend class mhe::PrivateEventListener<MouseInputAspect>;
typedef boost::shared_ptr<gui_listener> gui_listener_ptr;
public:
	MouseInputAspect(const std::string& name,
					 boost::shared_ptr<iInputSystem> input_system,
					 const rect<int>& geom) :
		Aspect(name),
		mouse_move_listener_(new gui_listener(mhe::MouseEventType,
											  mhe::MOUSE_MOVE, 0,
											  this, &MouseInputAspect::handle_mouse_move)),
		mouse_click_listener_(new gui_listener(mhe::MouseEventType,
											   mhe::MOUSE_BUTTON_PRESSED, 0,
											   this, &MouseInputAspect::handle_mouse_click)),
		mouse_on_(false),
		geom_(geom)
	{
		input_system->addListener(mouse_move_listener_);
		input_system->addListener(mouse_click_listener_);
	}
private:
	void do_subscribe(Aspect*) {}

	bool update_impl(int, const void*) {return true;}

	bool handle_mouse_move(const mhe::Event& e)
	{
		const mhe::MouseEvent& me = dynamic_cast<const mhe::MouseEvent&>(e);
		bool mouse_on = geom_.in(me.position());
		
		int event = Aspect::invalid_type;
		if (mouse_on_ && !mouse_on) event = mouse_left_event;
		else if (!mouse_on_ && mouse_on) event = mouse_on_event;
		mouse_on_ = mouse_on;
		if (event == Aspect::invalid_type)
		{
			if (mouse_on_) event = mouse_move_int_event;
			else return false;
		}
		update_childs(event, &me);	
		return true;
	}

	bool handle_mouse_click(const mhe::Event& e)
	{
		const mhe::MouseEvent& me = dynamic_cast<const mhe::MouseEvent&>(e);
		bool mouse_on = geom_.in(me.position());
		if (!mouse_on) return false;
		mouse_on_ = true;
		update_childs(mouse_click_event, &me);
		return true;
	}

	gui_listener_ptr mouse_move_listener_;
	gui_listener_ptr mouse_click_listener_;	
	bool mouse_on_;
	mhe::rect<int> geom_;
};

}}

#endif
