#ifndef __QT_WINDOW_SYSTEM_HPP__
#define __QT_WINDOW_SYSTEM_HPP__

#include <QApplication>
#include <QTimer>
#include "render/window_system.hpp"
#include "qt_view.hpp"

namespace mhe {
namespace qt {

class QtMainLoop : public QObject, public MainLoop
{
	Q_OBJECT
	public:		
	QtMainLoop(QtView* view) :
	view_(view)
	{
		init_events();
	}

	void run(unsigned int update_interval)
	{
		timer_.start(update_interval);
		QApplication::instance()->exec();
	}

    void stop()
    {
        QApplication::instance()->exit(0);
        timer_.stop();
    }
private:
	void init_events()
	{
		QObject::connect(&timer_, SIGNAL(timeout()),
						 this, SLOT(on_timer()));
	}
private slots:
	void on_timer()
	{
        if (view_->events_handler() != nullptr)
            view_->events_handler()->on_update();
		view_->updateGL();
	}

private:
	QTimer timer_;
	QtView* view_;
};

class QtWindowSystem : public WindowSystemImpl
{
public:
	QtWindowSystem();
	bool init(const vector2<int>& r, int bpp, bool fullscreen);
	bool set_format(const WindowContextFormat& format);
	void close();
	void set_caption(const std::string& caption);
	void swap_buffers();
	void show_cursor(bool /*show*/) {}
	void resize(const vector2<int>&) {}

	View* view()
	{
		return view_;
	}

	void set_view(View* view)
	{
		assert(dynamic_cast<QtView*>(view) != nullptr);
		view_ = static_cast<QtView*>(view);
	}

	MainLoop* main_loop() const
	{
		return get_main_loop();
	}
private:
	MainLoop* get_main_loop() const
	{
		if (main_loop_ == nullptr)
			main_loop_.reset(new QtMainLoop(view_));
		return main_loop_.get();
	}

	QtView* view_;
	mutable unique_ptr<QtMainLoop> main_loop_;
};

}}

#endif
