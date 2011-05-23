#include "engine.hpp"

namespace mhe  { 
namespace game {

	Engine::Engine() :
		running_(false)
	{}
	
	bool Engine::init(cmn::uint w, cmn::uint h, cmn::uint bpp, bool fullscreen)
	{
		if (!ws_.init(w, h, bpp, fullscreen))
			return false;
		is_.setWindowSystem(&ws_);
		// init quit event listener
		is_.addListener(new QuitListener(this, mhe::SystemEventType, mhe::QUIT));
		
		// graphics driver initialization
		driver_.reset(new mhe::OpenGLDriver());
		driver_.set_window_system(&ws_);
		return true;
	}
	
	void Engine::run()
	{
		running_ = true;
		while (running_)
		{
			is_.check();
			scene_.draw(driver_);
			scene_.update(is_.getTick());
		}
	}
	
	void Engine::stop()
	{
		running_ = false;
	}

}
}
