#ifndef __SYSTEM_FACTORY_HPP__
#define __SYSTEM_FACTORY_HPP__

#include <boost/shared_ptr.hpp>

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
	InputSystem* create_input_system() const;
	Texture* create_texture() const;
	Texture* create_multitexture() const;
	iAudioDriver* create_audio_driver() const;
	iSound* create_sound() const;

	void set_system_factory(iSystemFactory* factory)
	{
		factory_.reset(factory);
	}	
private:
	SystemFactory() {}
	SystemFactory(const SystemFactory&) {}
	~SystemFactory() {}
	SystemFactory& operator= (const SystemFactory&) {return *this;}

	boost::shared_ptr<iSystemFactory> factory_;
};
}

#endif
