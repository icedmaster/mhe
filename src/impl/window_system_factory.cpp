#include "impl/window_system_factory.hpp"

#include "platform/platform.hpp"

namespace mhe {

WindowSystemFactory::WindowSystemFactory()
{
	#ifdef MHE_SDL
	systems_.push_back(boost::shared_ptr<AbstractWindowSystemFactory>(new sdl::SDLWindowSystemFactory));
	#endif

	#ifdef MHE_QT
	systems_.push_back(boost::shared_ptr<AbstractWindowSystemFactory>(new QtWindowSystemFactory));
	#endif

	current_system_factory_ = systems_.back();
}

std::vector<std::string> WindowSystemFactory::available_systems_list() const
{
	std::vector<std::string> names;
	for (size_t i = 0; i < systems_.size(); ++i)
		names.push_back(systems_[i]->name());
	return names;
}

std::string WindowSystemFactory::current_system_name() const
{
	return current_system_factory_->name();
}

WindowSystemImpl* WindowSystemFactory::create_window_system() const
{
	return current_system_factory_->create_window_system();
}

KeyboardDeviceImpl* WindowSystemFactory::create_keyboard_device() const
{
	return current_system_factory_->create_keyboard_device_impl();
}

MouseDeviceImpl* WindowSystemFactory::create_mouse_device() const
{
	return current_system_factory_->create_mouse_device_impl();
}

SystemDeviceImpl* WindowSystemFactory::create_system_device() const
{
	return current_system_factory_->create_system_device_impl();
}

EventSystem* WindowSystemFactory::create_event_system() const
{
	return current_system_factory_->create_event_system();
}

}
