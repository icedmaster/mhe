#ifndef __IOS_MOUSE_DEVICE_IMPL_HPP__
#define __IOS_MOUSE_DEVICE_IMPL_HPP__

#include "events/mouse_device_impl.hpp"

namespace mhe {
namespace ios {

class iOSMouseDeviceImpl : public MouseDeviceImpl
{
public:
	std::vector< boost::shared_ptr<Event> > check(const WindowSystem& window_system)
	{
		return std::vector< boost::shared_ptr<Event> >();
	}
};

}}

#endif
