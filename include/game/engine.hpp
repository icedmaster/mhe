#ifndef _ENGINE_HPP_
#define _ENGINE_HPP_

#include "mhe.hpp"
#include "game_scene.hpp"

namespace mhe
{
	namespace game
	{
		class Engine
		{
			private:
				WindowSystem ws_;
				InputSystem is_;
				TextureManager tm_;
				FontManager fm_;
				SoundManager sm_;
				boost::shared_ptr<iDriver> driver_;
				boost::shared_ptr<iAudioDriver> audio_driver_;

				boost::shared_ptr<GameScene> game_scene_;

				bool running_;		   

				typedef PrivateEventListener<Engine> EngineEventListener;
				friend class PrivateEventListener<Engine>;
				bool stop_p(const Event&)
				{
					stop();
					return true;
				}

				void default_setup();
			public:
				Engine();
			
				bool init(cmn::uint w, cmn::uint h, cmn::uint bpp, bool fullscreen = false);
				void run();
				void stop();

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
			
				InputSystem& getInputSystem()
				{
					return is_;
				}
			
				boost::shared_ptr<iDriver> getDriver() const
				{
					return driver_;
				}

				boost::shared_ptr<iAudioDriver> getAudioDriver() const
				{
					return audio_driver_;
				}

				void setGameScene(boost::shared_ptr<GameScene> scene)
				{
					game_scene_ = scene;
				}
		};
	}	// game
}       // mhe

#endif
