#ifndef __MOUSE_DEVICE_HPP__
#define __MOUSE_DEVICE_HPP__

#include "device.hpp"
#include "core/array.hpp"
#include "mouse_device_impl.hpp"

namespace mhe {

class MHE_EXPORT MouseDevice : public Device
{
public:
	static const size_t max_buttons_number = 16; 
public:
	MouseDevice(const std::string& name);	

	const vector2<int>& position() const
	{
		return position_;
	}

	const vector2<int>& delta() const
	{
		return delta_;
	}

	const vector2<float>& wheel_delta() const
	{
		return wheel_delta_;
	}

	bool is_button_pressed(int button) const
	{
		return buttons_[button];
	}
private:
	void check_impl(events_vector& events, const WindowSystem&);

	array<bool, max_buttons_number> buttons_;
	vector2<int> position_;
	vector2<int> delta_;
	vector2<float> wheel_delta_;
	unique_ptr<MouseDeviceImpl> impl_;
};

}

#endif
