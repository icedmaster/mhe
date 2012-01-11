#include "game/engine.hpp"

namespace mhe  { 
namespace game {

Engine::Engine() :
	driver_(SystemFactory::instance().createDriver()),
	audio_driver_(SystemFactory::instance().createAudioDriver()),
	is_(SystemFactory::instance().createInputSystem()),
	running_(false),
	quit_listener_(new EngineEventListener(mhe::SystemEventType, mhe::QUIT, 0, 
											this, &Engine::stop_p))
{}
	
bool Engine::init(cmn::uint w, cmn::uint h, cmn::uint bpp, bool fullscreen)
{
	if (!ws_.init(w, h, bpp, fullscreen))
		return false;
	is_->setWindowSystem(&ws_);
	// init quit event listener
	is_->addListener(quit_listener_);
		
	// graphics driver initialization
	driver_->set_window_system(&ws_);
	driver_->init();

	default_setup();

	// audio driver initialization
	audio_driver_->init();

	// set helpers for managers
	tm_.set_helper(driver_);
	fm_.set_helper(driver_);
	sm_.set_helper(audio_driver_);
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
		is_->check();
		driver_->clear_depth();
		driver_->clear_color();

		if (game_scene_ && game_scene_->process())
		{
			game_scene_->getScene()->update(utils::get_current_tick());
			game_scene_->getScene()->draw(driver_);			
		}

		update();
		
		ws_.swapBuffers();
	}
}
	
void Engine::stop()
{
	running_ = false;
}

void Engine::update()
{
	cmn::uint now = utils::get_current_tick();
	aspect_manager_.update(now);
}

void Engine::free_all()
{
	tm_.free_all();
	fm_.free_all();
	sm_.free_all();
}

void Engine::set_next_scene(const std::string& arg)
{	
	game_scene_->deinit();
	game_scene_ = game_scene_->getNextScene();
	if (game_scene_) game_scene_->init(arg);	
}

}}
