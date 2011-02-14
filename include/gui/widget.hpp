#ifndef _WIDGET_HPP_
#define _WIDGET_HPP_

#include "input.hpp"
#include "font.hpp"
#include "rect.hpp"
#include "irenderable.hpp"

namespace mhe
{
	namespace gui
	{
		class Widget;

		enum WidgetType
		{
			MainWidgetType,
			LabelType,
			ButtonType
		};

		// abstract Widget class
		class Widget : public iRenderable
		{
			private:
				virtual bool handle_keyboard_event(const KeyboardEvent&) = 0;
				virtual bool handle_mouse_event(const MouseEvent&) = 0;
				virtual const boost::shared_ptr<Widget>& get_parent() const = 0;
				virtual void add_widget(const boost::shared_ptr<Widget>&) = 0;
				virtual void set_name(const std::string&) = 0;
				virtual const std::string& get_name() const = 0;
				virtual WidgetType get_type() const = 0;
				virtual void set_geom(const rect&) = 0;
				virtual void set_font(const boost::shared_ptr<iFont>&) = 0;
				virtual void set_image(const std::string&) {}
				virtual void draw_impl() {};
				virtual void set_on_mouse_move(EventListener*) {}
				virtual void set_on_mouse_left(EventListener*) {}
				virtual void set_on_mouse_click(EventListener*) {}
			public:
				// events
				bool handleKeyboardEvent(const KeyboardEvent& e)
				{
					return handle_keyboard_event(e);
				}

				bool handleMouseEvent(const MouseEvent& e)
				{
					return handle_mouse_event(e);
				}

				const boost::shared_ptr<Widget>& parent() const
				{
					return get_parent();
				}

				void add(const boost::shared_ptr<Widget>& w)
				{
					add_widget(w);
				}

				void setName(const std::string& s)
				{
					set_name(s);
				}

				const std::string& name() const
				{
					return get_name();
				}

				WidgetType type() const
				{
					return get_type();
				}

				void setGeometry(const rect& r)
				{
				    set_geom(r);
				}

				void setFont(const boost::shared_ptr<iFont>& fnt)
				{
				    set_font(fnt);
				}

				void setImage(const std::string& filename)
				{
				    set_image(filename);
				}

				void setOnMouseMove(EventListener* el)
				{
				    set_on_mouse_move(el);
				}

				void setOnMouseLeft(EventListener* el)
				{
				    set_on_mouse_left(el);
				}

				void setOnMouseClick(EventListener* el)
				{
				    set_on_mouse_click(el);
				}
		};

	};	// gui
};	// mhe

#endif
