#ifndef _LABEL_HPP_
#define _LABEL_HPP_

#include "widget.hpp"
#include "itexture.hpp"
#include <map>

namespace mhe
{
	namespace gui
	{
		class Label : public Widget
		{
			private:
				rect<float> g;		// geometry
				bool mouse_on_;
				boost::shared_ptr<iTexture> texture_;
				std::map <int, guieventptr> handlers;
				bool enabled_;
				boost::shared_ptr<iFont> font_;
				std::wstring caption_;

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

				void set_enabled(bool enable)
				{
					enabled_ = enable;
				}

				void set_font(const boost::shared_ptr<iFont>& font)
				{
					font_ = font;
				}

				void set_caption(const std::wstring& caption)
				{
					caption_ = caption;
				}
			public:
                Label();

				void setTexture(const boost::shared_ptr<iTexture>& t)
				{
					texture_ = t;
				}
		};
	}
}

#endif
