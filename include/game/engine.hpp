#ifndef _ENGINE_HPP_
#define _ENGINE_HPP_

#include "mhe.hpp"
#include "game_scene.hpp"
#include "aspect_manager.hpp"
#include "timed_events.hpp"
#include "mhe_event.hpp"

namespace mhe {
namespace game {

class Engine
{
public:
	Engine();
			
	bool init(cmn::uint w, cmn::uint h, cmn::uint bpp, bool fullscreen = false);
	void run();
	void stop();
	void process();
	void resize(cmn::uint w, cmn::uint h);

	bool is_running() const
	{
		return running_;
	}
			
	TextureManager& texture_manager()
	{
		return tm_;
	}
			
	/*FontManager& font_manager()
	{
		return fm_;
		}*/

	SoundManager& sound_manager()
	{
		return sm_;
	}

	WindowSystem& window_system()
	{
		return ws_;
	}
			
	EventManager& input_system()
	{
		return event_manager_;
	}
			
	boost::shared_ptr<Driver> driver() const
	{
		return driver_;
	}

	boost::shared_ptr<iAudioDriver> audio_driver() const
	{
		return audio_driver_;
	}

	void set_game_scene(boost::shared_ptr<GameScene> scene)
	{
		game_scene_ = scene;
	}

	void free_all();
	void set_next_scene(const std::string& arg);

	boost::shared_ptr<GameScene> get_game_scene() const
	{
		return game_scene_;
	}

	AspectManager& get_aspect_manager()
	{
		return aspect_manager_;
	}

	TimedEventsManager& get_timed_events_manager()
	{
		return timed_events_manager_;
	}

	bool initialized() const
	{
		return initialized_;
	}
private:
	friend class EventListener;
	bool stop_p(const Event&)
	{
		stop();
		return true;
	}

	void default_setup();
	void update();

	WindowSystem ws_;
	TextureManager tm_;
	//FontManager fm_;
	SoundManager sm_;
	EventManager event_manager_;
	boost::shared_ptr<Driver> driver_;
	boost::shared_ptr<iAudioDriver> audio_driver_;

	boost::shared_ptr<GameScene> game_scene_;

	AspectManager aspect_manager_;
	TimedEventsManager timed_events_manager_;

	bool running_;
	bool initialized_;		   
};

}}       // namespaces

#endif
