#ifndef __KEYBOARD_DEVICE_IMPL_HPP__
#define __KEYBOARD_DEVICE_IMPL_HPP__

#include <vector>
#include <boost/shared_ptr.hpp>

namespace mhe {

class Event;
class KeyboardEvent;
class WindowSystem;

class KeyboardDeviceImpl
{
public:
	virtual ~KeyboardDeviceImpl() {}
	virtual std::vector< boost::shared_ptr<Event> > check(const WindowSystem&) = 0;
};

}

#endif
