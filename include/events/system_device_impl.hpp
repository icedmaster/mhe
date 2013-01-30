#ifndef __SYSTEM_DEVICE_IMPL_HPP__
#define __SYSTEM_DEVICE_IMPL_HPP__

#include <vector>
#include <boost/shared_ptr.hpp>
#include "device.hpp"

namespace mhe {

class Event;
class SystemEvent;
class WindowSystem;

class SystemDeviceImpl
{
public:
	virtual ~SystemDeviceImpl() {}
	virtual void check(Device::events_vector& events, const WindowSystem&) = 0;
};

}

#endif
