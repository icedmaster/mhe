#ifndef __IOS_SYSTEM_DEVICE_IMPL_HPP__
#define __IOS_SYSTEM_DEVICE_IMPL_HPP__

#include "events/system_device_impl.hpp"

namespace mhe {
namespace ios {

class iOSSystemDeviceImpl : public SystemDeviceImpl
{
public:
	void check(Device::events_vector& /*events*/, const WindowSystem&)
	{
	}
};

}}

#endif
