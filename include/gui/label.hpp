#ifndef _LABEL_HPP_
#define _LABEL_HPP_

#include "gui/widget.hpp"
#include "gui/widget_impl.hpp"
#include "impl/system_factory.hpp"
#include <boost/scoped_ptr.hpp>

namespace mhe
{
    namespace gui
    {
        class Label : public Widget
        {
            private:
                boost::shared_ptr<Widget> parent_;
                boost::scoped_ptr<LabelImpl> impl_;
                std::string name_;

                boost::shared_ptr<EventListener> on_mouse_move_;
                boost::shared_ptr<EventListener> on_mouse_click_;
                boost::shared_ptr<EventListener> on_mouse_left_;
                bool mouse_on_;
            private:
                bool handle_keyboard_event(const KeyboardEvent&)
                {
                    return false;
                }

				bool handle_mouse_event(const MouseEvent& me);

				const boost::shared_ptr<Widget>& get_parent() const
				{
				    return parent_;
				}

				void add_widget(const boost::shared_ptr<Widget>&)
				{
				    return;
				}

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
				    return LabelType;
				}

				void set_geom(const rect& r)
				{
				    impl_->setGeometry(r);
				}

				void set_font(const boost::shared_ptr<iFont>& f)
				{
				    impl_->setFont(f);
				}

				void draw_impl()
				{
				    impl_->draw();
				}

				void set_on_mouse_click(EventListener* el)
                {
                    on_mouse_click_.reset(el);
                }

                void set_on_mouse_move(EventListener* el)
                {
                    on_mouse_move_.reset(el);
                }

                void set_on_mouse_left(EventListener* el)
                {
                    on_mouse_left_.reset(el);
                }

            public:
                Label() :
                    impl_(GUIFactory::instance().createLabel()),
                    mouse_on_(false)
                {
                }

                void setCaption(const std::string& caption)
                {
                    impl_->setCaption(caption);
                }

                void setBackground(const colorf& color)
                {
                    impl_->setBackground(color);
                }
        };
    };
};

#endif
