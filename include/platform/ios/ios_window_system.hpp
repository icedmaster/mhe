#ifndef __IOS_WINDOW_SYSTEM_HPP__
#define __IOS_WINDOW_SYSTEM_HPP__

#include "window_system.hpp"

namespace mhe {
namespace ios {

class iOSWindowSystemImpl;

class iOSWindowSystem : public WindowSystemImpl
{
public:
	iOSWindowSystem();
	bool init(const vector2<int>& r, int bpp, bool fullscreen);
	void close();
	void set_caption(const std::string& /*caption*/)
	{}
	void swap_buffers();
	void show_cursor(bool /*show*/)
	{}
	// TODO:
	void resize(const vector2<int>&) {}

	View* view();
private:
	boost::scoped_ptr<iOSWindowSystemImpl> impl_;
};

}}

#endif
