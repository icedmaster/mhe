#ifndef _WIDGET_HPP_
#define _WIDGET_HPP_

#include "mhe_math.hpp"
#include "video_driver.hpp"
#include "event.hpp"
#include "ifont.hpp"
#include "sprite.hpp"
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
			    virtual ~GUIEventHandler() {}
				virtual void handle(const Widget*) {}
		};
		typedef boost::shared_ptr<GUIEventHandler> guieventptr;

		// helper class
		template <class T>
		class PrivateGUIEventHandler : public GUIEventHandler
		{
			typedef void (T::*Handler)(const Widget*);
			T* object_;
			Handler handler_;
		public:
			PrivateGUIEventHandler(T* object, Handler handler) :
				object_(object),
				handler_(handler)
			{}

			void handle(const Widget* widget)
			{
				assert(object_ != 0);
				(object_->*handler_)(widget);
			}
		};

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
				bool enabled_;
				bool mouse_on_;
                colorf clr;
				std::string name_;
				std::wstring caption_;
				boost::shared_ptr<Sprite> sprite_;
				rect<float> geom_;
				boost::shared_ptr<iFont> font_;
				std::map <int, guieventptr> handlers_;
			private:
				void process_event(int e)
				{
					std::map<int, guieventptr>::iterator it = handlers_.find(e);
					if ( (it != handlers_.end()) && it->second )
                    	it->second->handle(this);
				}
			private:
				virtual void draw_impl(boost::shared_ptr<Driver> driver)
				{
					draw_rect(driver);
				}

                virtual void set_parent(const Widget*) {}
				virtual widgetptr get_parent() const {return widgetptr();}
				virtual void add_child(const widgetptr&) {}
				virtual std::vector<widgetptr> get_childs() const
				{
					return std::vector<widgetptr>(0);
				}

				// events
				virtual void set_mouse_on(bool on)  {mouse_on_ = on;}
				virtual bool is_mouse_on() const {return mouse_on_;}
				
				virtual bool is_handler_supported(int) const = 0;
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

				virtual void draw_rect(const boost::shared_ptr<Driver>& driver);                                       
			public:
				Widget();
				virtual ~Widget() {}

				void set_geometry(const rect<float>& r)
				{
					geom_ = r;
				}

				const rect<float>& geometry() const
				{
					return geom_;
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

				void draw(boost::shared_ptr<Driver> driver)
				{
					draw_impl(driver);
				}

				bool handleMouseClick(const MouseEvent& me)
				{
					if (geom_.in(me.position()))
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
					if (geom_.in(me.position()))
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

				void set_handler(int event, GUIEventHandler* handler)
				{
					set_handler(event, guieventptr(handler));
				}

				void set_handler(int event, const guieventptr& handler);

				void set_visible(bool visible)
				{
					visible_ = visible;
				}

				bool is_visible() const
				{
					return visible_;
				}

				void set_enabled(bool enabled)
				{
					enabled_ = enabled;
				}

				bool is_enabled() const
				{
					return enabled_;
				}

				void set_font(boost::shared_ptr<iFont> font)
				{
					font_ = font;
				}

				void set_caption(const std::wstring& caption)
				{
					caption_ = caption;
				}

				virtual void set_color(const colorf& c)
				{
				    clr = c;
				}

				virtual colorf get_color() const
				{
					return clr;
				}


			void set_name(const std::string& name)
			{
				name_ = name;
			}

			const std::string& get_name() const
			{
				return name_;
			}

			void set_sprite(boost::shared_ptr<Sprite> sprite)
			{
				sprite_ = sprite;				
			}

			boost::shared_ptr<Sprite> get_sprite() const
			{
				return sprite_;
			}

		};
	}
}

#endif
