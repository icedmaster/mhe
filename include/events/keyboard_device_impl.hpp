#ifndef __KEYBOARD_DEVICE_IMPL_HPP__
#define __KEYBOARD_DEVICE_IMPL_HPP__

#include <vector>
#include <boost/shared_ptr.hpp>

namespace mhe {

class KeyboardEvent;

class KeyboardDeviceImpl
{
public:
	virtual ~KeyboardDeviceImpl() {}
	virtual std::vector< boost::shared_ptr<KeyboardEvent> > check() = 0;
};

}

#endif
