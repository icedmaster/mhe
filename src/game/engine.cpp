#include "game/engine.hpp"

#include "game/base_view_events_handler.hpp"
#include "events/system_device.hpp"
#include "utils/sysutils.hpp"
#include "impl/system_factory.hpp"

#include "utils/profiling.hpp"

namespace mhe  { 
namespace game {

Engine::Engine() :
	audio_driver_(SystemFactory::instance().create_audio_driver()),
	running_(false), initialized_(false)
{
	// init static context members
	context_.set_window_system(&ws_);
}
	
bool Engine::init(cmn::uint w, cmn::uint h, cmn::uint bpp, bool fullscreen)
{
	if (initialized_) return true;
	INFO_LOG("Engine init:" << w << " " << h << " " << bpp);
	while (true)
	{
		WindowContextFormat format;
		format.major_version = context_.driver().major_version_need();
		format.minor_version = context_.driver().minor_version_need();
		if (!ws_.init(w, h, bpp, fullscreen, format))
		{
			ERROR_LOG("Can't initialize WindowSystem with format:" << format.major_version << " " <<
					  format.minor_version);
			const std::string& name = SystemFactory::instance().video_driver_factory().set_next_driver();
			if (name.empty())
			{
				ERROR_LOG("All available drivers has been tried - can't initialize engine");
				return false;
			}
			INFO_LOG("Try next driver with name:" << name);
			context_.driver().reset();
			continue;
		}
		else
		{
			INFO_LOG("Initialized WindowSystem with format:" << format.major_version << " " <<
					  format.minor_version);
			break;
		}
	}
	ws_.view()->set_events_handler(new BaseViewEventsHandler(this));
	// init quit event listener
	event_manager_.add_device(new SystemDevice("sys"));
	event_manager_.add_listener(new DelegateEventListener(system_event_type, SystemEvent::quit, Event::any_event,
						      create_delegate(this, &Engine::stop_p)));
		
	// graphics driver initialization
	context_.driver().set_window_system(&ws_);
	context_.driver().init();

	default_setup();

	// audio driver initialization
	if (audio_driver_ != nullptr)
		audio_driver_->init();

	sm_.set_helper(audio_driver_);
	fm_.set_helper(context_);
	initialized_ = true;
	return true;
}

void Engine::default_setup()
{
	ws_.show_cursor(true);
	context_.driver().set_clear_color(mhe::color_black);
	context_.driver().enable_depth((mhe::DepthFunc)0);
	context_.driver().disable_lighting();
	context_.driver().set_viewport(0, 0, context_.window_system().width(), context_.window_system().height());
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
	update();
	render();
}

void Engine::update()
{
	ADD_PROFILE_ELEMENT("Engine::update");
	event_manager_.check(ws_);
	update_internal();

	if (game_scene_ && game_scene_->process())
		game_scene_->scene()->update(utils::get_current_tick());
}

void Engine::render()
{
	ADD_PROFILE_ELEMENT("Engine::render");
	context_.update();	
	game_scene_->scene()->draw(context_);
	game_scene_->draw();
	context_.flush();
}
	
void Engine::stop()
{
	running_ = false;
}

void Engine::resize(cmn::uint w, cmn::uint h)
{
	ws_.resize(w, h);
	context_.driver().set_viewport(0, 0, w, h);
}

void Engine::update_internal()
{
	cmn::uint now = utils::get_current_tick();
	component_manager_.update(now, this);
}

void Engine::free_all()
{
	context_.reset();
	//fm_.free_all();
	sm_.free_all();
}

}}
