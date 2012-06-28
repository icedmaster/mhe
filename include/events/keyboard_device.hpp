#ifndef __KEYBOARD_DEVICE_HPP__
#define __KEYBOARD_DEVICE_HPP__

#include <boost/scoped_ptr.hpp>
#include "device.hpp"
#include "keyboard_device_impl.hpp"

namespace mhe {

class KeyboardDevice : public Device
{
public:
	KeyboardDevice(const std::string& name);
	bool is_key_pressed(int sym) const;
private:
	std::vector< boost::shared_ptr<Event> > check_impl(const WindowSystem&);

	boost::scoped_ptr<KeyboardDeviceImpl> impl_;
	bool keys_[256];
};

}

#endif
