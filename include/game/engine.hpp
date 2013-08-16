#ifndef _ENGINE_HPP_
#define _ENGINE_HPP_

#include "game_scene.hpp"
#include "component_manager.hpp"
#include "mhe_event.hpp"
#include "context.hpp"
#include "sound_manager.hpp"
#include "sound/iaudio_driver.hpp"
#include "gui/font_manager.hpp"

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
	void update();
	void render();
	void resize(cmn::uint w, cmn::uint h);

	bool is_running() const
	{
		return running_;
	}	

	Context& context()
	{
		return context_;
	}	   

	const Context& context() const
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

	boost::shared_ptr<GameScene> game_scene() const
	{
		return game_scene_;
	}

	ComponentManager& component_manager()
	{
		return component_manager_;
	}

	bool initialized() const
	{
		return initialized_;
	}

	gui::FontManager& font_manager()
	{
		return fm_;
	}
private:
	friend class EventListener;
	bool stop_p(const Event*)
	{
		stop();
		return true;
	}

	void default_setup();
	void update_internal();

	WindowSystem ws_;
	Context context_;
	gui::FontManager fm_;
	SoundManager sm_;
	EventManager event_manager_;
	boost::shared_ptr<iAudioDriver> audio_driver_;

	boost::shared_ptr<GameScene> game_scene_;

	ComponentManager component_manager_;

	bool running_;
	bool initialized_;		   
};

}}       // namespaces

#endif
