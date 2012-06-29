#ifndef __WIDGET_HPP__
#define __WIDGET_HPP__

#include "event_widget.hpp"
#include "mhe_math.hpp"
#include "sprite.hpp"
#include "font.hpp"
#include "video_driver.hpp"

namespace mhe {
namespace gui {

// external classes
class GUINode;

class Widget;
typedef boost::shared_ptr<Widget> widgetptr;

class Widget : public EventWidget
{
	friend class GUINode;
public:
	Widget();
	Widget(const std::string& name);
	virtual ~Widget() {}

	// accessors
	void set_name(const std::string& name)
	{
		name_ = name;
	}

	const std::string& name() const
	{
		return name_;
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

	void set_caption(const std::wstring& caption)
	{
		caption_ = caption;
	}

	const std::wstring& caption() const
	{
		return caption_;
	}

	void set_font(const boost::shared_ptr<Font>& font)
	{
		font_ = font;
	}

	const boost::shared_ptr<Font>& font() const
	{
		return font_;
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

	void add_widget(const widgetptr& widget);
	widgetptr get_widget(const std::string& name) const;
	Widget* parent() const
	{
		return parent_;
	}
protected:
	void draw(const Context& context);
private:
	// hierarhy
	Widget* parent_;
	std::map<std::string, widgetptr> children_;	
	// draw
	boost::shared_ptr<Sprite> sprite_;
	boost::shared_ptr<Sprite> highlighted_sprite_;
	// basic widget parameters
	std::string name_;
	// text parameters
	std::wstring caption_;
	boost::shared_ptr<Font> font_;
	// flags
	bool visible_;
	bool enabled_;
};

}}

#endif
