#ifndef _MAIN_WIDGET_HPP_
#define _MAIN_WIDGET_HPP_

#include "widget.hpp"
#include <map>

namespace mhe
{
	namespace gui
	{
		class MainWidget : public Widget
		{
			private:
				boost::shared_ptr<Widget> parent_;
				std::string name_;
				std::map< std::string, boost::shared_ptr<Widget> > childs_;
			private:
				void handle_keyboard_event(const KeyboardEvent&);
				void handle_mouse_event(const MouseEvent&);
				
				const boost::shared_ptr<Widget>& get_parent() const
				{
					return parent_;
				}
				
				void add_widget(const boost::shared_ptr<Widget>& w);									
				
				void set_name(const std::string& name)
				{
					name_ = name;
				}
				
				const std::string& get_name() const
				{
					return name_;
				}
				
				WidgetType get_type() const
				{
					return MainWidget;
				}
				
				void draw_impl() {}
		};
	};
};

#endif

			