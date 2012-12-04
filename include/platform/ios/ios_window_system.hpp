#ifndef __IOS_WINDOW_SYSTEM_HPP__
#define __IOS_WINDOW_SYSTEM_HPP__

#include "window_system.hpp"

#import <GLKit/GLKit.h>

namespace mhe {
namespace ios {

class iOSWindowSystem : public WindowSystemImpl
{
public:
	bool init(const vector2<int>& r, int bpp, bool fullscreen);
	void close();
	void set_caption(const std::string& /*caption*/)
	{}
	void swap_buffers();
	void show_cursor(bool show)
	{}
	// TODO:
	void resize(const vector2<int>&) {}
private:
	
};

}}

#endif
