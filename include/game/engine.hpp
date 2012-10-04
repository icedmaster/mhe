#ifndef _ENGINE_HPP_
#define _ENGINE_HPP_

#include "mhe.hpp"
#include "game_scene.hpp"
#include "aspect_manager.hpp"
#include "mhe_event.hpp"
#include "context.hpp"

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

	Context& context()
	{
		return context_;
	}	   

	SoundManager& sound_manager()
	{
		return sm_;
	}
			
	EventManager& event_manager()
	{
		return event_manager_;
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

	boost::shared_ptr<GameScene> game_scene() const
	{
		return game_scene_;
	}

	AspectManager& aspect_manager()
	{
		return aspect_manager_;
	}

	bool initialized() const
	{
		return initialized_;
	}
private:
	friend class EventListener;
	bool stop_p(const Event*)
	{
		stop();
		return true;
	}

	void default_setup();
	void update();

	WindowSystem ws_;
	Context context_;
	//FontManager fm_;
	SoundManager sm_;
	EventManager event_manager_;
	boost::shared_ptr<iAudioDriver> audio_driver_;

	boost::shared_ptr<GameScene> game_scene_;

	AspectManager aspect_manager_;

	bool running_;
	bool initialized_;		   
};

}}       // namespaces

#endif
