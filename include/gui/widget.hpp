#ifndef __WIDGET_HPP__
#define __WIDGET_HPP__

#include "gui_event_type.hpp"
#include "delegate.hpp"
#include "mhe_math.hpp"
#include "mhe_event.hpp"
#include "sprite.hpp"
#include "font.hpp"
#include "video_driver.hpp"

namespace mhe {
namespace gui {

// external classes
class GUINode;

class Widget;
typedef boost::shared_ptr<Widget> widgetptr;

class Widget
{
	friend class GUINode;
	typedef Delegate1<void, Widget*> Delegate;
public:
	typedef DelegateFunc1<void, Widget*> EventHandler;
public:
	Widget();
	Widget(const std::string& name);
	virtual ~Widget() {}

	void set_geom(const rect<float>& geom)
	{
		geom_ = geom;
	}

	const rect<float>& geom() const
	{
		return geom_;
	}

	// accessors
	void set_name(const std::string& name)
	{
		name_ = name;
	}

	const std::string& name() const
	{
		return name_;
	}

	void set_sprite(Sprite* sprite)
	{
		set_sprite(boost::shared_ptr<Sprite>(sprite));
	}

	void set_sprite(const boost::shared_ptr<Sprite>& sprite)
	{
		sprite_ = sprite;
	}

	const boost::shared_ptr<Sprite>& sprite() const
	{
		return sprite_;
	}

	void set_highlighted_sprite(const boost::shared_ptr<Sprite>& sprite)
	{
		highlighted_sprite_ = sprite;
	}

	const boost::shared_ptr<Sprite>& highlighted_sprite() const
	{
		return highlighted_sprite_;
	}

	void set_caption(const utf32_string& caption)
	{
		caption_ = caption;
		update_caption();
	}

	const utf32_string& caption() const
	{
		return caption_;
	}

	void set_font(const boost::shared_ptr<Font>& font)
	{
		font_ = font;
		update_caption();
	}

	const boost::shared_ptr<Font>& font() const
	{
		return font_;
	}

	void set_caption_color(const colorf& color)
	{
		caption_color_ = color;
        update_caption();
	}

	const colorf& caption_color() const
	{
		return caption_color_;
	}

	void set_visible(bool visible)
	{
		visible_ = visible;
	}

	bool visible() const
	{
		return visible_;
	}

	void set_enabled(bool enabled)
	{
		enabled_ = enabled;
	}

	bool enabled() const
	{
		return enabled_;
	}

	bool mouse_on() const
	{
		return mouse_on_;
	}

	bool button_pressed() const
	{
		return button_pressed_;
	}

	void add_handler(int event, EventHandler* handler);

	void add_widget(const widgetptr& widget);
	void add_widget(Widget* widget);
	widgetptr get_widget(const std::string& name) const;
	Widget* parent() const
	{
		return parent_;
	}
protected:
	void draw(const Context& context);
	void draw_impl(const boost::shared_ptr<Sprite>& sprite, const Context& content);
private:
	vector2<float> relative_position() const
	{
		if (parent_ == nullptr)
			return geom().ll();
		return vector2<float>(parent_->geom().ll().x() + geom().ll().x(),
							  parent_->geom().ll().y() + geom().ll().y());
	}

	void update_caption();

	void process_mouse_move_event(const MouseEvent* event);
	void process_mouse_press_event(const MouseEvent* event);
	void process_mouse_release_event(const MouseEvent* event);
	void process_event(int event, const MouseEvent* mouse_event);

	// hierarhy
	Widget* parent_;
	std::vector<widgetptr> children_;	
	// draw
	boost::shared_ptr<Sprite> sprite_;
	boost::shared_ptr<Sprite> highlighted_sprite_;
	// basic widget parameters
	std::string name_;
	// text parameters
	utf32_string caption_;
	boost::shared_ptr<Font> font_;
	colorf caption_color_;
	boost::scoped_ptr<Renderable> caption_renderable_;
	// flags
	bool visible_;
	bool enabled_;
	// events
	rect<float> geom_;
	bool mouse_on_;
	bool button_pressed_;
	std::map<int, Delegate> handlers_;
};

}}

#endif
