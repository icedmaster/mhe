#include "game/engine.hpp"

#include "events/system_device.hpp"

namespace mhe  { 
namespace game {

Engine::Engine() :
	audio_driver_(SystemFactory::instance().create_audio_driver()),
	running_(false), initialized_(false)
{}
	
bool Engine::init(cmn::uint w, cmn::uint h, cmn::uint bpp, bool fullscreen)
{
	if (initialized_) return true;
	INFO_LOG("Engine init:" << w << " " << h << " " << bpp);
	if (!ws_.init(w, h, bpp, fullscreen))
		return false;	
	boost::shared_ptr<Driver> driver(SystemFactory::instance().create_driver());
	context_.set_window_system(&ws_);
	context_.set_driver(driver);
	// init quit event listener
	event_manager_.add_device(new SystemDevice("sys"));
	event_manager_.add_listener(new DelegateEventListener(system_event_type, SystemEvent::quit, Event::any_event,
						      create_delegate(this, &Engine::stop_p)));
		
	// graphics driver initialization
	driver->set_window_system(&ws_);
	driver->init();

	default_setup();

	// audio driver initialization
	if (audio_driver_ != nullptr)
		audio_driver_->init();

	sm_.set_helper(audio_driver_);
	initialized_ = true;
	return true;
}

void Engine::default_setup()
{
	ws_.show_cursor(true);
	context_.driver()->set_clear_color(mhe::color_black);
	context_.driver()->enable_depth((mhe::DepthFunc)0);
	context_.driver()->disable_lighting();
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
	context_.driver()->clear_depth();
	context_.driver()->clear_color();

	if (game_scene_ && game_scene_->process())
	{
		context_.driver()->begin_render();
		game_scene_->get_scene()->update(utils::get_current_tick());
		game_scene_->get_scene()->draw(context());			
		context_.driver()->end_render();
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
	context_.driver()->set_viewport(0, 0, w, h);
}

void Engine::update()
{
	cmn::uint now = utils::get_current_tick();
	aspect_manager_.update(now);
}

void Engine::free_all()
{
	context_.reset();
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
