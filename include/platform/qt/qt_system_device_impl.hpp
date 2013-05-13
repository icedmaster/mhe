#ifndef __QT_SYSTEM_DEVICE_IMPL_HPP__
#define __QT_SYSTEM_DEVICE_IMPL_HPP__

#include "events/system_device_impl.hpp"

namespace mhe {
namespace qt {

class QtSystemDeviceImpl : public SystemDeviceImpl
{
public:
	void check(Device::events_vector& /*events*/, const WindowSystem&)
	{}
};

}}

#endif
