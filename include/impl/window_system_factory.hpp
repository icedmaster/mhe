#ifndef __WINDOW_SYSTEM_FACTORY_HPP__
#define __WINDOW_SYSTEM_FACTORY_HPP__

#include <vector>
#include <boost/shared_ptr.hpp>
#include "abstract_window_system_factory.hpp"

namespace mhe {

class WindowSystemFactory
{
public:
	WindowSystemFactory();

	std::vector<std::string> available_systems_list() const;
	std::string current_system_name() const;

	WindowSystemImpl* create_window_system() const;
	KeyboardDeviceImpl* create_keyboard_device() const;
	MouseDeviceImpl* create_mouse_device() const;
	SystemDeviceImpl* create_system_device() const;
private:
	std::vector< boost::shared_ptr<AbstractWindowSystemFactory> > systems_;
	boost::shared_ptr<AbstractWindowSystemFactory> current_system_factory_;
};

}

#endif
