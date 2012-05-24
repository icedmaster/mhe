#ifndef _ENGINE_HPP_
#define _ENGINE_HPP_

#include "mhe.hpp"
#include "game_scene.hpp"
#include "aspect_manager.hpp"
#include "timed_events.hpp"

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
			
	TextureManager& getTextureManager()
	{
		return tm_;
	}
			
	FontManager& getFontManager()
	{
		return fm_;
	}

	SoundManager& getSoundManager()
	{
		return sm_;
	}

	WindowSystem& getWindowSystem()
	{
		return ws_;
	}
			
	boost::shared_ptr<InputSystem> getInputSystem() const
	{
		return is_;
	}
			
	boost::shared_ptr<Driver> getDriver() const
	{
		return driver_;
	}

	boost::shared_ptr<iAudioDriver> getAudioDriver() const
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
	typedef PrivateEventListener<Engine> EngineEventListener;
	friend class PrivateEventListener<Engine>;
	bool stop_p(const Event&)
	{
		stop();
		return true;
	}

	void default_setup();
	void update();

	WindowSystem ws_;
	TextureManager tm_;
	FontManager fm_;
	SoundManager sm_;
	boost::shared_ptr<Driver> driver_;
	boost::shared_ptr<iAudioDriver> audio_driver_;
	boost::shared_ptr<InputSystem> is_;

	boost::shared_ptr<GameScene> game_scene_;

	AspectManager aspect_manager_;
	TimedEventsManager timed_events_manager_;

	bool running_;
	bool initialized_;		   

	boost::shared_ptr<EngineEventListener> quit_listener_;
};

}}       // namespaces

#endif
