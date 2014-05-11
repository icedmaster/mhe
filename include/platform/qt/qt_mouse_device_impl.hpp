#ifndef __QT_MOUSE_DEVICE_IMPL_HPP__
#define __QT_MOUSE_DEVICE_IMPL_HPP__

#include "events/mouse_device_impl.hpp"

namespace mhe {
namespace qt {

class QtMouseDeviceImpl : public MouseDeviceImpl
{
public:
	void check(Device::events_vector& events, const WindowSystem& window_system);
};

}}

#endif
