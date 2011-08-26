#include "gui/widget.hpp"

namespace mhe {
namespace gui {

	Widget::Widget() :
		visible_(true),
		clr(cfWhite)
	{}

	void Widget::draw_rect(const boost::shared_ptr<iDriver>& driver,
						   const boost::shared_ptr<iTexture>& texture)
	{
		if (!visible_) return;

		const rect<float>& g = get_geometry();
		const colorf& clr = get_color();
		const boost::shared_ptr<iFont>& font = get_font();

		float v[12] = {g.ll().x(), g.ll().y(), 0,
					   g.ll().x(), g.rh().y(), 0,
					   g.rh().x(), g.rh().y(), 0,
                       g.rh().x(), g.ll().y(), 0};
		const float n[] = {0.0, 0.0, 1.0,
                           0.0, 0.0, 1.0,
                           0.0, 0.0, 1.0,
                           0.0, 0.0, 1.0};
		const float c[] = {clr.r(), clr.g(), clr.b(), clr.a(),
                           clr.r(), clr.g(), clr.b(), clr.a(),
                           clr.r(), clr.g(), clr.b(), clr.a(),
                           clr.r(), clr.g(), clr.b(), clr.a()};
		const float t[] = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0};
        const cmn::uint i[] = {0, 1, 2, 2, 3, 0};

		const float* tc = (texture) ? t : 0;

		driver->mask_zbuffer();
        driver->enable_blending(ALPHA_ONE_MINUS_ALPHA);

		if (texture)
			texture->prepare();
		driver->draw(v, n, tc, c, i, 6);
		if (texture)
			texture->clean();

		if (font)
		{
			v2d coord(g.ll().x(), g.ll().y() + g.height() / 2);
			font->print(driver, get_caption(), coord);
		}

		driver->unmask_zbuffer();
        driver->disable_blending();
	}

}
}
