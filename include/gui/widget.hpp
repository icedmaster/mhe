#ifndef _WIDGET_HPP_
#define _WIDGET_HPP_

#include "mhe_math.hpp"
#include "idriver.hpp"
#include "event.hpp"
#include "ifont.hpp"
#include "itexture.hpp"
#include <boost/shared_ptr.hpp>

namespace mhe
{
	namespace gui
	{
	    class Widget;
		typedef boost::shared_ptr<Widget> widgetptr;

		class GUIEventHandler
		{
			public:
				virtual void handle(const Widget*) {}
		};
		typedef boost::shared_ptr<GUIEventHandler> guieventptr;

		enum
		{
			OnMouseMove = 0,
			OnMouseLeft = 1,
			OnMouseLeftClick = 2,
			OnMouseRightClick = 3,
			OnMouseButtonRelease = 4
		};

		// Must be implemented next functions:
		// set_geometry(rect&)
		// get_geometry()
		// draw_impl()
		// set_visible(bool)
		// get_visible()
		// set_enabled(bool)

		class Widget
		{
			private:
				bool visible_;
                colorf clr;
			private:
				void process_event(int e)
				{
					guieventptr ge = get_gui_handler(e);
					if (ge)
                    	ge->handle(this);
				}
			private:
				virtual void set_geometry(const rect<float>&) = 0;
				virtual const rect<float>& get_geometry() const = 0;

				virtual void draw_impl(const boost::shared_ptr<iDriver>&) = 0;

                virtual void set_parent(const Widget*) {}
				virtual widgetptr get_parent() const {return widgetptr();}
				virtual void add_child(const widgetptr&) {}
				virtual std::vector<widgetptr> get_childs() const
				{
					return std::vector<widgetptr>(0);
				}

				// events
				virtual void set_mouse_on(bool)  {}
				virtual bool is_mouse_on() const {return false;}

				virtual void set_handler(int, const guieventptr&) {}
				virtual guieventptr get_gui_handler(int) const {return guieventptr();}

				virtual void set_enabled(bool) = 0;

				virtual void set_font(const boost::shared_ptr<iFont>&) {}
				virtual boost::shared_ptr<iFont> get_font() const {return boost::shared_ptr<iFont>();}
				virtual void set_caption(const std::wstring&) {}
				virtual std::wstring get_caption() const {return std::wstring();}

				virtual void set_color(const colorf& c)
				{
				    clr = c;
				}

				virtual colorf get_color() const
				{
					return clr;
				}
			protected:
				// can be called from inherited classes
				virtual void process_mouse_left_click(const MouseEvent&)
				{
					process_event(OnMouseLeftClick);
				}

				virtual void process_mouse_right_click(const MouseEvent&)
				{
					process_event(OnMouseRightClick);
				}

				virtual void process_mouse_move(const MouseEvent&)
				{
					process_event(OnMouseMove);
				}

				virtual void process_mouse_left(const MouseEvent&)
				{
					process_event(OnMouseLeft);
				}

				virtual void process_mouse_button_release(const MouseEvent&)
				{
					process_event(OnMouseButtonRelease);
				}

				virtual void draw_rect(const boost::shared_ptr<iDriver>& driver,
                                       const boost::shared_ptr<iTexture>& texture);

                virtual void set_visible(bool visible)
				{
					visible_ = visible;
				}

				virtual bool get_visible() const
				{
					return visible_;
				}

			public:
				Widget();
				virtual ~Widget() {}

				void setGeometry(const rect<float>& r)
				{
					set_geometry(r);
				}

				const rect<float>& geometry() const
				{
					return get_geometry();
				}

				void add(const widgetptr& widget)
				{
					widget->setParent(this);
					add_child(widget);
				}

				void setParent(const Widget* parent)
				{
				    set_parent(parent);
				}

				void draw(const boost::shared_ptr<iDriver>& driver)
				{
					draw_impl(driver);
				}

				bool handleMouseClick(const MouseEvent& me)
				{
					if (geometry().in(me.position()))
					{
						const std::vector<widgetptr>& ch = get_childs();
						for (size_t i = 0; i < ch.size(); ++i)
						{
							if (ch[i]->handleMouseClick(me))
								return true;
						}
						set_mouse_on(true);
						if ( (me.state() == MOUSE_BUTTON_RELEASED) || !me.button() )
							process_mouse_button_release(me);
						else if (me.button() == BUTTON_LEFT)
							process_mouse_left_click(me);
						else if (me.button() == BUTTON_RIGHT)
							process_mouse_right_click(me);
						return true;
					}
					else if (is_mouse_on())
					{
						set_mouse_on(false);
						process_mouse_left(me);
					}
					return false;
				}

				bool handleMouseMove(const MouseEvent& me)
				{
					if (geometry().in(me.position()))
					{
						const std::vector<widgetptr>& ch = get_childs();
						for (size_t i = 0; i < ch.size(); ++i)
						{
							if (ch[i]->handleMouseMove(me))
								return true;
						}
						process_mouse_move(me);
						set_mouse_on(true);
						return true;
					}
					else if (is_mouse_on())
					{
						set_mouse_on(false);
						process_mouse_left(me);
					}
					return false;
				}

				void setHandler(int event, GUIEventHandler* handler)
				{
					set_handler(event, guieventptr(handler));
				}

				void setHandler(int event, const guieventptr& handler)
				{
					set_handler(event, handler);
				}

				void setVisible(bool visible)
				{
					set_visible(visible);
				}

				bool isVisible() const
				{
					return get_visible();
				}

				void setEnabled(bool enable)
				{
					set_enabled(enable);
				}

				void setFont(const boost::shared_ptr<iFont>& font)
				{
					set_font(font);
				}

				void setCaption(const std::wstring& caption)
				{
					set_caption(caption);
				}

				void setColor(const colorf& c)
				{
				    set_color(c);
				}
		};
	}
}

#endif
