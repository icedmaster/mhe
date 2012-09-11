#ifndef __CONTEXT_HPP__
#define __CONTEXT_HPP__

#include "video_driver.hpp"
#include "window_system.hpp"
#include "texture_manager.hpp"

namespace mhe {

class Context
{
public:
	void set_driver(const boost::shared_ptr<Driver>& driver)
	{
		driver_ = driver;
		texture_manager_.set_helper(driver_);
	}

	boost::shared_ptr<Driver> driver() const
	{
		return driver_;
	}

	void set_window_system(WindowSystem* window_system)
	{
		window_system_ = window_system;
	}

	WindowSystem window_system() const
	{
		return *window_system_;
	}

	const TextureManager& texture_manager() const
	{
		return texture_manager_;
	}

	void reset()
	{
		// TODO: implement full reset
		texture_manager_.free_all();
	}
private:
	boost::shared_ptr<Driver> driver_;
	WindowSystem* window_system_;
	TextureManager texture_manager_;
};

}

#endif
