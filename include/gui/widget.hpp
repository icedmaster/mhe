#ifndef _WIDGET_HPP_
#define _WIDGET_HPP_

#include "input.hpp"
#include "font.hpp"

namespace mhe
{
	namespace gui
	{
		class Widget;
		
		enum WidgetType
		{
			MainWidget,
			Label,
			Button
		};
		
		// abstract Widget class
		class iWidget
		{
			private:
				virtual void handle_keyboard_event(const KeyboardEvent&) = 0;
				virtual void handle_mouse_event(const MouseEvent&) = 0;
				virtual const boost::shared_ptr<Widget>& get_parent() const = 0; 
				virtual void add_widget(const boost::shared_ptr<Widget>&) = 0;
				virtual void set_name(const std::string&) = 0;
				virtual const std::string& get_name() const = 0; 
				virtual WidgetType get_type() const = 0;
				virtual void draw_impl() = 0;
			public:
				// events
				void handleKeyboardEvent(const KeyboardEvent& e)
				{
					handle_keyboard_event(e);
				}
				
				void handleMouseEvent(const MouseEvent& e)
				{
					handle_mouse_event(e);
				}
				
				const boost::shared_ptr<Widget>& parent() const
				{
					return get_parent();
				}
				
				void add(const boost::shared_ptr& w)
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
				
				void draw()
				{
					draw_impl();
				}
		};
		
	};	// gui
};	// mhe

#endif
