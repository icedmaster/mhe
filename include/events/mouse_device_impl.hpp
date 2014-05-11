#ifndef __MOUSE_DEVICE_IMPL_HPP__
#define __MOUSE_DEVICE_IMPL_HPP__

#include <vector>
#include <boost/shared_ptr.hpp>
#include "event.hpp"
#include "device.hpp"

namespace mhe {

class Event;
class MouseEvent;
class WindowSystem;

class MouseDeviceImpl
{
public:
	virtual ~MouseDeviceImpl() {}
	virtual void check(Device::events_vector& events, const WindowSystem&) = 0;
};

}

#endif
