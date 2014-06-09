#ifndef __SYSTEM_DEVICE_HPP__
#define __SYSTEM_DEVICE_HPP__

#include "core/unique_ptr.hpp"
#include "device.hpp"
#include "system_device_impl.hpp"

namespace mhe {

class SystemDevice : public Device
{
public:
	SystemDevice(const std::string& name);
private:
	void check_impl(events_vector& events, const WindowSystem&);

	unique_ptr<SystemDeviceImpl> impl_;
};

}

#endif
