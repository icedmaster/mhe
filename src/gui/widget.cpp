#include "gui/widget.hpp"
#include "utils/sysutils.hpp"

namespace mhe {
namespace gui {

	Widget::Widget() :
		visible_(true),
		enabled_(true),
		mouse_on_(false),
		clr(cfWhite)
	{}

	void Widget::draw_rect(const boost::shared_ptr<Driver>& driver)						   
	{
		if (!visible_) return;

		sprite_->setSize(geom_.width(), geom_.height());
		sprite_->identity();
		sprite_->translate(v3d(geom_.ll().x(), geom_.ll().y(), 0));
		sprite_->draw(driver);
		if (font_)
		{
			v2d coord(geom_.ll().x(), geom_.ll().y() + geom_.height() / 2);
			font_->print(driver, caption_, coord);
		}
	}

	void Widget::set_handler(int event, const guieventptr& handler)
	{
		if (!is_handler_supported(event)) return;
		handlers_[event] = handler;
	}

}
}
