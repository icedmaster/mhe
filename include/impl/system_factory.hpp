#ifndef __SYSTEM_FACTORY_HPP__
#define __SYSTEM_FACTORY_HPP__

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include "input_factory.hpp"
#include "video_driver_factory.hpp"
#include "window_system_factory.hpp"
#include "types.hpp"

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

	Driver* create_driver() const;
	WindowSystemImpl* create_window_system() const;
	Texture* create_texture() const;
	iAudioDriver* create_audio_driver() const;
	iSound* create_sound() const;

	// input devices
	KeyboardDeviceImpl* create_keyboard_device_impl() const
	{
		if (input_factory_ == nullptr) return nullptr;
		return input_factory_->create_keyboard_device_impl();
	}

	MouseDeviceImpl* create_mouse_device_impl() const
	{
		return input_factory_->create_mouse_device_impl();
	}

	SystemDeviceImpl* create_system_device_impl() const
	{
		return input_factory_->create_system_device_impl();
	}

	void set_input_factory(InputFactory* factory)
	{
		input_factory_ .reset(factory);
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

	boost::scoped_ptr<InputFactory> input_factory_;
	VideoDriverFactory video_driver_factory_;
	WindowSystemFactory window_system_factory_;
};
}

#endif
