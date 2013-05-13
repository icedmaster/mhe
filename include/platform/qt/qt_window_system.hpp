#ifndef __QT_WINDOW_SYSTEM_HPP__
#define __QT_WINDOW_SYSTEM_HPP__

#include "window_system.hpp"
#include "qt_view.hpp"

namespace mhe {
namespace qt {

class QtWindowSystem : public WindowSystemImpl
{
public:
	bool init(const vector2<int>& r, int bpp, bool fullscreen);
	void close();
	void set_caption(const std::string& caption);
	void swap_buffers();
	void show_cursor(bool /*show*/) {}
	void resize(const vector2<int>&) {}

	View* view()
	{
		return view_;
	}

	void set_view(QtView* view)
	{
		view_ = view;
	}
private:
	QtView* view_;
};

}}

#endif
