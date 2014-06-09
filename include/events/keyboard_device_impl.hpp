#ifndef __KEYBOARD_DEVICE_IMPL_HPP__
#define __KEYBOARD_DEVICE_IMPL_HPP__

#include "device.hpp"

namespace mhe {

class Event;
class KeyboardEvent;
class WindowSystem;

class KeyboardDeviceImpl
{
public:
	virtual ~KeyboardDeviceImpl() {}
	virtual void check(Device::events_vector& events, const WindowSystem&) = 0;
};

}

#endif
