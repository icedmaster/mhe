#ifndef _LABEL_HPP_
#define _LABEL_HPP_

#include "widget.hpp"
#include <map>

namespace mhe
{
	namespace gui
	{
		class Label : public Widget
		{
			private:
				bool is_handler_supported(int event) const
				{
					static int supported[] = {OnMouseMove, OnMouseLeft, OnMouseLeftClick,
											  OnMouseRightClick};
					if (std::find(supported, supported + 4, event) == (supported + 4))
						return false;
					return true;
				}
		};
	}
}

#endif
