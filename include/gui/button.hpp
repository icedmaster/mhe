#ifndef _BUTTON_HPP_
#define _BUTTON_HPP_

#include "widget.hpp"
#include <map>

namespace mhe
{
	namespace gui
	{
		class Button : public Widget
		{
			bool is_handler_supported(int event) const;
			void process_mouse_left_click(const MouseEvent&);
			void process_mouse_button_release(const MouseEvent&);
			void process_mouse_left(const MouseEvent&);
		};
	}
}

#endif
