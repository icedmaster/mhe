#ifndef __QT_UTILS_HPP__
#define __QT_UTILS_HPP__

#include <QWidget>
#include "mhe_math.hpp"
#include "events/mouse_event.hpp"

namespace mhe {
namespace qt {
namespace utils {

inline int get_mouse_button(QMouseEvent* event)
{
	if (event->button() == Qt::NoButton)
		return Event::any_event;
	return (event->button() == Qt::LeftButton) ? MouseEvent::left_button : MouseEvent::right_button;
}

inline vector2<float> get_mouse_position(QMouseEvent* event)
{
	return vector2<float>(event->x(), event->y());
}

inline int get_keyboard_key(QKeyEvent* event)
{
	int key = event->key();
	if ( (key >= Qt::Key_A) && (key <= Qt::Key_Z) )
		key += 32;
	return key;
}

}}}

#endif
