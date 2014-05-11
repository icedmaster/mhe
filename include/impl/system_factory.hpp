#ifndef __SYSTEM_FACTORY_HPP__
#define __SYSTEM_FACTORY_HPP__

#include "input_factory.hpp"
#include "video_driver_factory.hpp"
#include "window_system_factory.hpp"
#include "core/types.hpp"

namespace mhe
{
class iAudioDriver;
class iSound;

class SystemFactory
{
public:
	static SystemFactory& instance()
	{
		static SystemFactory sf;
		return sf;
	}

	DriverImpl* create_driver() const;
	WindowSystemImpl* create_window_system() const;
	Texture* create_texture() const;
	iAudioDriver* create_audio_driver() const;
	iSound* create_sound() const;

	// input devices
	KeyboardDeviceImpl* create_keyboard_device_impl() const
	{
		return window_system_factory_.create_keyboard_device();
	}

	MouseDeviceImpl* create_mouse_device_impl() const
	{
		return window_system_factory_.create_mouse_device();
	}

	SystemDeviceImpl* create_system_device_impl() const
	{
		return window_system_factory_.create_system_device();
	}

	EventSystem* create_event_system() const
	{
		return window_system_factory_.create_event_system();
	}

	VideoDriverFactory& video_driver_factory()
	{
		return video_driver_factory_;
	}

	WindowSystemFactory& window_system_factory()
	{
		return window_system_factory_;
	}
private:
	SystemFactory() {}
	SystemFactory(const SystemFactory&) {}
	~SystemFactory() {}
	SystemFactory& operator= (const SystemFactory&) {return *this;}

	VideoDriverFactory video_driver_factory_;
	WindowSystemFactory window_system_factory_;
};
}

#endif
