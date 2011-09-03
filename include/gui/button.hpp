#ifndef _BUTTON_HPP_
#define _BUTTON_HPP_

#include "widget.hpp"
#include "itexture.hpp"
#include <map>

namespace mhe
{
	namespace gui
	{
		class Button : public Widget
		{
			private:
				rect<float> g;		// geometry
				bool mouse_on_;				
				boost::shared_ptr<iTexture> texture_;
				boost::shared_ptr<iTexture> pressed_texture_;
				std::map <int, guieventptr> handlers;
				bool enabled_;
				bool pressed_;

				static const int supported_handlers_number = 4;
				static int supported_handlers[supported_handlers_number];
			private:
				void set_mouse_on(bool on)
				{
					mouse_on_ = on;
				}

				bool is_mouse_on() const
				{
					return mouse_on_;
				}

				void set_geometry(const rect<float>& r)
				{
					g = r;
				}

				const rect<float>& get_geometry() const
				{
					return g;
				}

				void draw_impl(const boost::shared_ptr<iDriver>& driver);

				void set_handler(int event, const guieventptr& handler);
				guieventptr get_gui_handler(int event) const;
				void process_mouse_left_click(const MouseEvent&);
				void process_mouse_button_release(const MouseEvent&);
				void process_mouse_left(const MouseEvent&);

				void set_enabled(bool enable)
				{
					enabled_ = enable;
				}
			public:
				Button();

				void setTexture(const boost::shared_ptr<iTexture>& t)
				{
					texture_ = t;
				}

				void setPressedTexture(const boost::shared_ptr<iTexture>& t)
				{
					pressed_texture_ = t;
				}

			boost::shared_ptr<iTexture> getTexture() const
			{
				return texture_;
			}
		};
	}
}

#endif
