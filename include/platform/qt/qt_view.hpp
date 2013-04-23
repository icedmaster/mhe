#ifndef __QT_VIEW_HPP__
#define __QT_VIEW_HPP__

#include <QtOpenGL>
#include "view.hpp"

namespace mhe {
namespace qt {

public QtView : public QGLWidget, public View
{
Q_OBJECT
public:
	QtView(QWidget* parent = nullptr) :
		QGLWidget(parent)
	{}

	QtView(ViewEventsHandler* events_handler, QWidget* parent = nullptr) :
		QGLWidget(parent), View(events_handler)
	{}

private:
	void paintGL()
	{
		on_draw();
	}
};

}}

#endif
