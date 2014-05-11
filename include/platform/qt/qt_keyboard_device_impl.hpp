#ifndef __QT_KEYBOARD_DEVICE_IMPL_HPP__
#define __QT_KEYBOARD_DEVICE_IMPL_HPP__

#include "events/keyboard_device_impl.hpp"

namespace mhe {
namespace qt {

class QtKeyboardDeviceImpl : public KeyboardDeviceImpl
{
public:
	void check(Device::events_vector& events, const WindowSystem&);
};

}}

#endif
