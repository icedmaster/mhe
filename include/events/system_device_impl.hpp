#ifndef __SYSTEM_DEVICE_IMPL_HPP__
#define __SYSTEM_DEVICE_IMPL_HPP__

#include <vector>
#include <boost/shared_ptr.hpp>

namespace mhe {

class Event;
class SystemEvent;
class WindowSystem;

class SystemDeviceImpl
{
public:
	virtual ~SystemDeviceImpl() {}
	virtual std::vector< boost::shared_ptr<Event> > check(const WindowSystem&) = 0;
};

}

#endif
