#ifndef _LABEL_HPP_
#define _LABEL_HPP_

#include "widget.hpp"
#include <map>

namespace mhe {
namespace gui {

class Label : public Widget
{
private:
	bool is_handler_supported(int event) const
	{
		static int supported[] = {on_mouse_move, on_mouse_left, on_mouse_left_click,
								  on_mouse_right_click};
		if (std::find(supported, supported + 4, event) == (supported + 4))
			return false;
		return true;
	}
};

}}

#endif
