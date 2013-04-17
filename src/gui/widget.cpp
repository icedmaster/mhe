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
	children_[widget->name()] = widget;
	widget->parent_ = this;
}

widgetptr Widget::get_widget(const std::string& name) const
{
	std::map<std::string, widgetptr>::const_iterator it = children_.find(name);
	if (it == children_.end()) return widgetptr();
	return it->second;
}

void Widget::draw(const Context& context)
{
	draw_impl(sprite_, context);
}

void Widget::draw_impl(const boost::shared_ptr<Sprite>& sprite, const Context& context)
{
	if (!visible_) return;
	sprite->set_size(geom().width(), geom().height());
	sprite->identity();
	sprite->translate(geom().ll().x(), geom().ll().y(), 0);
	sprite->draw(context);
}

}}
