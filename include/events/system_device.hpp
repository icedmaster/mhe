#ifndef __SYSTEM_DEVICE_HPP__
#define __SYSTEM_DEVICE_HPP__

#include <boost/scoped_ptr.hpp>
#include "device.hpp"
#include "system_device_impl.hpp"

namespace mhe {

class SystemDevice : public Device
{
public:
	SystemDevice(const std::string& name);
private:
	std::vector< boost::shared_ptr<Event> > check_impl(const WindowSystem&);

	boost::scoped_ptr<SystemDeviceImpl> impl_;
};

}

#endif
