#ifndef __CONTEXT_HPP__
#define __CONTEXT_HPP__

#include "video_driver.hpp"
#include "window_system.hpp"
#include "texture_manager.hpp"
#include "texture_atlas_manager.hpp"

namespace mhe {

class Context
{
public:
	void set_driver(const boost::shared_ptr<Driver>& driver)
	{
		driver_ = driver;
		texture_manager_.set_helper(driver_);
		texture_atlas_manager_.set_helper(&texture_manager_);
	}

	boost::shared_ptr<Driver> driver() const
	{
		return driver_;
	}

	void set_window_system(WindowSystem* window_system)
	{
		window_system_ = window_system;
	}

	const WindowSystem& window_system() const
	{
		return *window_system_;
	}
    
    WindowSystem& window_system()
    {
        return *window_system_;
    }

	const TextureManager& texture_manager() const
	{
		return texture_manager_;
	}
    
    TextureManager& texture_manager()
    {
        return texture_manager_;
    }

	const TextureAtlasManager& texture_atlas_manager() const
	{
		return texture_atlas_manager_;
	}

	void reset()
	{
		// TODO: implement full reset
		texture_manager_.free_all();
	}

	void update();
	void flush();
private:
	boost::shared_ptr<Driver> driver_;
	WindowSystem* window_system_;
	TextureManager texture_manager_;
	TextureAtlasManager texture_atlas_manager_;
};

}

#endif
