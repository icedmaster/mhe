#ifndef __QT_VIEW_HPP__
#define __QT_VIEW_HPP__

#include <QtOpenGL>
#include "app/view.hpp"
#include "events/events_provider.hpp"

namespace mhe {
namespace qt {

class QtView : public QGLWidget, public View
{
Q_OBJECT
public:
	QtView(QWidget* parent = nullptr) :
		QGLWidget(parent)
	{}

	QtView(ViewEventsHandler* events_handler, QWidget* parent = nullptr) :
		QGLWidget(parent), View(events_handler)
	{}

	EventsProvider* events_provider() const
	{
		return &events_provider_;
	}
private:
	void paintGL()
	{
		on_draw();
	}

	void resizeGL(int w, int h)
	{
		if (events_handler() != nullptr)
			events_handler()->on_resize(w, h);
	}

	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);

	mutable EventsProvider events_provider_;
};

}}

#endif
