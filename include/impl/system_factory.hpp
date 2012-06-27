#ifndef __SYSTEM_FACTORY_HPP__
#define __SYSTEM_FACTORY_HPP__

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include "input_factory.hpp"
#include "types.hpp"

namespace mhe
{
class Driver;
class iWindowSystem;
class InputSystem;
class Texture;
class iAudioDriver;
class iSound;

// if we want to use custom factory
class iSystemFactory
{
public:
	virtual ~iSystemFactory() {}
	virtual iWindowSystem* create_window_system() const = 0;
	virtual InputSystem* create_input_system() const = 0;
};

class SystemFactory
{
public:
	static SystemFactory& instance()
	{
		static SystemFactory sf;
		return sf;
	}

	Driver* create_driver() const;
	iWindowSystem* create_window_system() const;
	Texture* create_texture() const;
	Texture* create_multitexture() const;
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

	void set_system_factory(iSystemFactory* factory)
	{
		factory_.reset(factory);
	}	

	void set_input_factory(InputFactory* factory)
	{
		input_factory_ .reset(factory);
	}
private:
	SystemFactory() {}
	SystemFactory(const SystemFactory&) {}
	~SystemFactory() {}
	SystemFactory& operator= (const SystemFactory&) {return *this;}

	boost::shared_ptr<iSystemFactory> factory_;
	boost::scoped_ptr<InputFactory> input_factory_;
};
}

#endif
