#ifndef _GAME_SCENE_HPP_
#define _GAME_SCENE_HPP_

#include "scene.hpp"

namespace mhe
{
	namespace game
	{
	class Engine;

		// base game scene class
		class GameScene
		{
		private:
			Engine* engine_;
			boost::shared_ptr<GameScene> prev_;
			boost::shared_ptr<GameScene> next_;
				
			boost::shared_ptr<Scene> scene_;
		private:
			virtual bool init_impl() {return true;}
		protected:
			Engine* get_engine() const
			{
				return engine_;
			}

			boost::shared_ptr<Scene> get_scene() const
			{
				return scene_;
			}
		public:
			GameScene(Engine* engine) :
				engine_(engine)
			{}

			virtual ~GameScene() {}
				
			bool init()
			{
				return init_impl();
			}
				
			void setNextScene(boost::shared_ptr<GameScene> gs)
			{
				next_ = gs;
			}
				
			boost::shared_ptr<GameScene> getNextScene() const
			{
				return next_;
			}
				
			void setPrevScene(boost::shared_ptr<GameScene> gs)
			{
				prev_ = gs;
			}
				
			boost::shared_ptr<GameScene> getPrevScene() const
			{
				return prev_;
			}

			void setScene(boost::shared_ptr<Scene> scene)
			{
				scene_ = scene;
			}
		};
	}
}

#endif
