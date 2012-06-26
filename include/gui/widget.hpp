#ifndef _WIDGET_HPP_
#define _WIDGET_HPP_

#include "mhe_math.hpp"
#include "video_driver.hpp"
#include "mhe_event.hpp"
#include "ifont.hpp"
#include "sprite.hpp"
#include <boost/shared_ptr.hpp>

namespace mhe {
namespace gui {

class Widget;
typedef boost::shared_ptr<Widget> widgetptr;

enum
{
	on_mouse_move = 0,
	on_mouse_left = 1,
	on_mouse_left_click = 2,
	on_mouse_right_click = 3,
	on_mouse_button_release = 4
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
public:
	typedef Delegate1<void, const Widget*> EventHandler;
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
		widget->set_parent(this);
		add_child(widget);
	}

	void set_parent(const Widget* parent)
	{
		set_parent(parent);
	}

	void draw(boost::shared_ptr<Driver> driver)
	{
		draw_impl(driver);
	}

	bool handle_mouse_click(const boost::shared_ptr<MouseEvent>& me)
	{
		if (geom_.in(me->pos()))
		{
			const std::vector<widgetptr>& ch = get_childs();
			for (size_t i = 0; i < ch.size(); ++i)
			{
				if (ch[i]->handle_mouse_click(me))
					return true;
			}
			set_mouse_on(true);
			if ( (me->event_type() == MouseEvent::button_released) || !me->button() )
				process_mouse_button_release(me);
			else if (me->button() == MouseEvent::left_button)
				process_mouse_left_click(me);
			else if (me->button() == MouseEvent::right_button)
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

	bool handle_mouse_move(const boost::shared_ptr<MouseEvent>& me)
	{
		if (geom_.in(me->pos()))
		{
			const std::vector<widgetptr>& ch = get_childs();
			for (size_t i = 0; i < ch.size(); ++i)
			{
				if (ch[i]->handle_mouse_move(me))
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

	void set_handler(int event, const EventHandler& handler);

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
		clr_ = c;
	}

	virtual colorf get_color() const
	{
		return clr_;
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
protected:
	// can be called from inherited classes
	virtual void process_mouse_left_click(const boost::shared_ptr<MouseEvent>&)
	{
		process_event(on_mouse_left_click);
	}

	virtual void process_mouse_right_click(const boost::shared_ptr<MouseEvent>&)
	{
		process_event(on_mouse_right_click);
	}

	virtual void process_mouse_move(const boost::shared_ptr<MouseEvent>&)
	{
		process_event(on_mouse_move);
	}

	virtual void process_mouse_left(const boost::shared_ptr<MouseEvent>&)
	{
		process_event(on_mouse_left);
	}

	virtual void process_mouse_button_release(const boost::shared_ptr<MouseEvent>&)
	{
		process_event(on_mouse_button_release);
	}

	virtual void draw_rect(const boost::shared_ptr<Driver>& driver);   
private:
	void process_event(int e)
	{
		std::map<int, EventHandler>::iterator it = handlers_.find(e);
		if (it != handlers_.end())
			it->second(this);
	}
private:
	virtual void draw_impl(boost::shared_ptr<Driver> driver)
	{
		draw_rect(driver);
	}

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
private:
	bool visible_;
	bool enabled_;
	bool mouse_on_;
	colorf clr_;
	std::string name_;
	std::wstring caption_;
	boost::shared_ptr<Sprite> sprite_;
	rect<float> geom_;
	boost::shared_ptr<iFont> font_;
	std::map <int, EventHandler> handlers_;

};
}}

#endif
