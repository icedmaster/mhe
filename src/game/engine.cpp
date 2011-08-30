#include "game/engine.hpp"

namespace mhe  { 
namespace game {

	Engine::Engine() :
		driver_(SystemFactory::instance().createDriver()),
		audio_driver_(SystemFactory::instance().createAudioDriver()),
		running_(false)
	{}
	
	bool Engine::init(cmn::uint w, cmn::uint h, cmn::uint bpp, bool fullscreen)
	{
		if (!ws_.init(w, h, bpp, fullscreen))
			return false;
		is_.setWindowSystem(&ws_);
		// init quit event listener
		is_.addListener(new EngineEventListener(mhe::SystemEventType, mhe::QUIT, 0, 
												this, &Engine::stop_p));
		
		// graphics driver initialization
		driver_->set_window_system(&ws_);

		default_setup();

		// audio driver initialization
		audio_driver_->init();
		return true;
	}

	void Engine::default_setup()
	{
		ws_.showCursor(true);
		//load_default_extensions();
		driver_->set_clear_color(mhe::cfBlack);
		driver_->enable_depth((mhe::DepthFunc)0);
		driver_->enable_lighting();
	}
	
	void Engine::run()
	{
		running_ = true;
		while (running_)
		{
			is_.check();
			driver_->clear_depth();
			driver_->clear_color();

			if (game_scene_ && game_scene_->process())
				game_scene_->getScene()->draw(driver_);			
			
			ws_.swapBuffers();
		}
	}
	
	void Engine::stop()
	{
		running_ = false;
	}

}
}
