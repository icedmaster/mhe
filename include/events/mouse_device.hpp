#ifndef __MOUSE_DEVICE_HPP__
#define __MOUSE_DEVICE_HPP__

#include <boost/scoped_ptr.hpp>
#include "device.hpp"
#include "mouse_device_impl.hpp"

namespace mhe {

class MouseDevice : public Device
{
public:
	MouseDevice(const std::string& name);
private:
	std::vector< boost::shared_ptr<Event> > check_impl(const WindowSystem&);

	boost::scoped_ptr<MouseDeviceImpl> impl_;
};

}

#endif
