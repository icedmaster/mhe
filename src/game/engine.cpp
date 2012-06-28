#include "game/engine.hpp"

#include "events/system_device.hpp"

namespace mhe  { 
namespace game {

Engine::Engine() :
	driver_(SystemFactory::instance().create_driver()),
	audio_driver_(SystemFactory::instance().create_audio_driver()),
	running_(false), initialized_(false)
{}
	
bool Engine::init(cmn::uint w, cmn::uint h, cmn::uint bpp, bool fullscreen)
{
	if (initialized_) return true;
	INFO_LOG("Engine init:" << w << " " << h << " " << bpp);
	if (!ws_.init(w, h, bpp, fullscreen))
		return false;	
	// init quit event listener
	event_manager_.add_device(new SystemDevice("sys"));
	event_manager_.add_listener(new EventListener(system_event_type, SystemEvent::quit, Event::any_event,
						      create_delegate(this, &Engine::stop_p)));
		
	// graphics driver initialization
	driver_->set_window_system(&ws_);
	driver_->init();

	default_setup();

	// audio driver initialization
	if (audio_driver_ != nullptr)
		audio_driver_->init();

	// set helpers for managers
	tm_.set_helper(driver_);
	//fm_.set_helper(driver_);
	sm_.set_helper(audio_driver_);
	initialized_ = true;
	return true;
}

void Engine::default_setup()
{
	ws_.show_cursor(true);
	//load_default_extensions();
	driver_->set_clear_color(mhe::cfBlack);
	driver_->enable_depth((mhe::DepthFunc)0);
	driver_->disable_lighting();
}
	
void Engine::run()
{
	running_ = true;
	while (running_)
	{
		process();
	}
}

void Engine::process()
{
	event_manager_.check(ws_);
	driver_->clear_depth();
	driver_->clear_color();

	if (game_scene_ && game_scene_->process())
	{
		game_scene_->get_scene()->update(utils::get_current_tick());
		game_scene_->get_scene()->draw(driver_);			
	}

	update();
		
	ws_.swap_buffers();
}
	
void Engine::stop()
{
	running_ = false;
}

void Engine::resize(cmn::uint w, cmn::uint h)
{
	ws_.resize(w, h);
	driver_->set_viewport(0, 0, w, h);
}

void Engine::update()
{
	cmn::uint now = utils::get_current_tick();
	aspect_manager_.update(now);
}

void Engine::free_all()
{
	tm_.free_all();
	//fm_.free_all();
	sm_.free_all();
}

void Engine::set_next_scene(const std::string& arg)
{	
	game_scene_->deinit();
	game_scene_ = game_scene_->get_next_scene();
	if (game_scene_) game_scene_->init(arg);	
}

}}
