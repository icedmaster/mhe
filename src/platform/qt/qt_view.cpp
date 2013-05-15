#include "platform/qt/qt_view.hpp"

#include "events/mouse_event.hpp"
#include "events/keyboard_event.hpp"
#include "platform/qt/qt_utils.hpp"

namespace mhe {
namespace qt {

void QtView::mousePressEvent(QMouseEvent* event)
{
	events_provider_.add_mouse_event(MouseEvent::button_pressed,
									 utils::get_mouse_button(event),
									 utils::get_mouse_position(event));
}

void QtView::mouseMoveEvent(QMouseEvent* event)
{
	events_provider_.add_mouse_event(MouseEvent::move,
									 utils::get_mouse_button(event),
									 utils::get_mouse_position(event));
}

void QtView::mouseReleaseEvent(QMouseEvent* event)
{
	events_provider_.add_mouse_event(MouseEvent::button_released,
									 utils::get_mouse_button(event),
									 utils::get_mouse_position(event));
}

void QtView::keyPressEvent(QKeyEvent* event)
{
	events_provider_.add_keyboard_event(KeyboardEvent::key_down, event->key());
}

void QtView::keyReleaseEvent(QKeyEvent* event)
{
	events_provider_.add_keyboard_event(KeyboardEvent::key_up, event->key());
}

}}
