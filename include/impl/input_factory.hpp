#ifndef __INPUT_FACTORY_HPP__
#define __INPUT_FACTORY_HPP__

namespace mhe {

class KeyboardDeviceImpl;
class MouseDeviceImpl;
class SystemDeviceImpl;

class InputFactory
{
public:
	virtual ~InputFactory() {}
	virtual KeyboardDeviceImpl* create_keyboard_device_impl() const = 0;
	virtual MouseDeviceImpl* create_mouse_device_impl() const = 0;
	virtual SystemDeviceImpl* create_system_device_impl() const = 0;
};

}

#endif
