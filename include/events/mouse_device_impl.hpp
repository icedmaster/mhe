#ifndef __MOUSE_DEVICE_IMPL_HPP__
#define __MOUSE_DEVICE_IMPL_HPP__

#include <vector>
#include <boost/shared_ptr.hpp>

namespace mhe {

class Event;
class MouseEvent;
class WindowSystem;

class MouseDeviceImpl
{
public:
	virtual ~MouseDeviceImpl() {}
	virtual std::vector< boost::shared_ptr<Event> > check(const WindowSystem&) = 0;
};

}

#endif
