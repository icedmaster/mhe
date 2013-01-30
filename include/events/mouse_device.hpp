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
	void check_impl(events_vector& events, const WindowSystem&);

	boost::scoped_ptr<MouseDeviceImpl> impl_;
};

}

#endif
