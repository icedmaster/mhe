#ifndef __BUTTON_HPP__
#define __BUTTON_HPP__

#include "widget.hpp"

namespace mhe {
namespace gui {

class Button : public Widget
{
public:
	Button(const std::string& name) :
		Widget(name)
	{}

	Button() :
		Widget()
	{}

    void set_pressed_sprite(Sprite* sprite)
    {
        set_pressed_sprite(boost::shared_ptr<Sprite>(sprite));
    }

	void set_pressed_sprite(const boost::shared_ptr<Sprite>& sprite)
	{
		pressed_sprite_ = sprite;
	}
private:
	void draw(const Context& context);
private:
	boost::shared_ptr<Sprite> pressed_sprite_;
};

}}

#endif
