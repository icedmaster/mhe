#include "gui/widget.hpp"

namespace mhe {
namespace gui {

Widget::Widget() :
	parent_(nullptr),
    caption_color_(color_black),
	visible_(true), enabled_(true),
	mouse_on_(false), button_pressed_(false)
{}

Widget::Widget(const std::string& name) :
	parent_(nullptr),
	name_(name),
    caption_color_(color_black),
	visible_(true), enabled_(true),
	mouse_on_(false), button_pressed_(false)
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
	// draw caption
	if (caption_renderable_ != nullptr)
	{
		caption_renderable_->identity();
		caption_renderable_->translate(position.x(), position.y(), 0);
		context.driver()->draw(caption_renderable_.get());
	}
}

void Widget::update_caption()
{
    if (font_ == nullptr) return;
	caption_renderable_.reset(font_->print(caption_, vector2<float>(0, 0), caption_color_));
}

void Widget::add_handler(int event, EventHandler* handler)
{
	handlers_[event] += handler;
}

void Widget::process_mouse_move_event(const MouseEvent* event)
{
	if (geom_.in(event->pos()))
	{
		if (!mouse_on_)
			process_event(mouse_move_in_event, event);
		mouse_on_ = true;
		process_event(mouse_move_event, event);
	}
	else if (mouse_on_)
	{
		mouse_on_ = false;
		process_event(mouse_move_out_event, event);
	}
}

void Widget::process_mouse_press_event(const MouseEvent* event)
{
	if (geom_.in(event->pos()))
	{
		button_pressed_ = true;
		process_event(mouse_button_press_event, event);
	}
}

void Widget::process_mouse_release_event(const MouseEvent* event)
{
	if (button_pressed_)
	{
		button_pressed_ = false;
		process_event(mouse_button_release_event, event);
	}
}

void Widget::process_event(int event, const MouseEvent* mouse_event)
{
	std::map<int, Delegate>::iterator it = handlers_.find(event);
	if (it == handlers_.end()) return;
	it->second(this);
}

}}
