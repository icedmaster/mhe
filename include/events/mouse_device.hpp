#ifndef __MOUSE_DEVICE_HPP__
#define __MOUSE_DEVICE_HPP__

#include <boost/scoped_ptr.hpp>
#include "device.hpp"
#include "array.hpp"
#include "mouse_device_impl.hpp"

namespace mhe {

class MouseDevice : public Device
{
public:
	static const size_t max_buttons_number = 16; 
public:
	MouseDevice(const std::string& name);	

	vector2<int> position() const
	{
		return position_;
	}

	bool is_button_pressed(int button) const
	{
		return buttons_[button];
	}
private:
	void check_impl(events_vector& events, const WindowSystem&);

	array<bool, max_buttons_number> buttons_;
	vector2<int> position_;	
	boost::scoped_ptr<MouseDeviceImpl> impl_;
};

}

#endif
