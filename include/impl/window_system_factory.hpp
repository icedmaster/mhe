#ifndef __WINDOW_SYSTEM_FACTORY_HPP__
#define __WINDOW_SYSTEM_FACTORY_HPP__

#include <vector>
#include "core/ref_ptr.hpp"
#include "core/compiler.hpp"
#include "abstract_window_system_factory.hpp"

namespace mhe {

class MHE_EXPORT WindowSystemFactory
{
public:
	WindowSystemFactory();

	std::vector<std::string> available_systems_list() const;
	std::string current_system_name() const;

	WindowSystemImpl* create_window_system() const;
	KeyboardDeviceImpl* create_keyboard_device() const;
	MouseDeviceImpl* create_mouse_device() const;
	SystemDeviceImpl* create_system_device() const;
	EventSystem* create_event_system() const;
private:
	std::vector< ref_ptr<AbstractWindowSystemFactory> > systems_;
	ref_ptr<AbstractWindowSystemFactory> current_system_factory_;
};

}

#endif
