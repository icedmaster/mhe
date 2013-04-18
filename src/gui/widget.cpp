#include "gui/widget.hpp"

namespace mhe {
namespace gui {

Widget::Widget() :
	parent_(nullptr),
	visible_(true), enabled_(true)
{}

Widget::Widget(const std::string& name) :
	parent_(nullptr),
	name_(name),
	visible_(true), enabled_(true)
{}

void Widget::add_widget(Widget* widget)
{
	add_widget(widgetptr(widget));
}

void Widget::add_widget(const widgetptr& widget)
{
	children_.push_back(widget);
	widget->parent_ = this;
}

widgetptr Widget::get_widget(const std::string& name) const
{
	for (size_t i = 0; i < children_.size(); ++i)
	{
		if (children_[i]->name() == name)
			return children_[i];
	}
	return widgetptr();
}

void Widget::draw(const Context& context)
{
	draw_impl(sprite_, context);
	for (size_t i = 0; i < children_.size(); ++i)
		children_[i]->draw(context);
}

void Widget::draw_impl(const boost::shared_ptr<Sprite>& sprite, const Context& context)
{
	if (!visible_) return;
	sprite->set_size(geom().width(), geom().height());
	sprite->identity();
	const vector2<float>& position = relative_position();
	sprite->translate(position.x(), position.y(), 0);
	sprite->draw(context);
}

}}
