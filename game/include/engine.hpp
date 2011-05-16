#ifndef _ENGINE_HPP_
#define _ENGINE_HPP_

#include "mhe.hpp"

namespace game
{
	class Engine
	{
		public:
			class QuitListener : public EventListener
			{
				private:
					Engine* engine_;
				public:
					QuitListener(Engine* engine, int event_type, int arg, int opt = 0) :
						EventListener(event_type, arg, opt),
						engine_(engine)
					{}
					
					bool handle(const Event& e)
					{
						engine_->stop();
						return true;
					}
			};
		private:
			WindowSystem ws_;
			InputSystem is_;
			TextureManager tm_;
			FontManager fm_;
			boost::shared_ptr<iDriver> driver_;
			bool running_;
			
			boost::shared_ptr<Scene> scene_;
		public:
			Engine();
			
			bool init(cmn::uint w = 800, cmn::uint h = 600, cmn::uint bpp = 32,
					  bool fullscreen = false);
			void run();
			void stop();
			
			TextureManager& getTextureManager() const
			{
				return tm_;
			}
			
			FontManager& getFontManager() const
			{
				return fm_;
			}
			
			InputSystem& getInputSystem() const
			{
				return is_;
			}
			
			boost::shared_ptr<iDriver>& getDriver() const
			{
				return driver_;
			}
	};
}

#endif
