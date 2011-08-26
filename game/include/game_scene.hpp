#ifndef _GAME_SCENE_HPP_
#define _GAME_SCENE_HPP_

#include "mhe.hpp"

namespace mhe
{
	namespace game
	{
		// base game scene class
		class GameScene : public SceneCallback
		{
			private:
				boost::shared_ptr<Engine> engine_;
				std::string init_arg_;
				boost::shared_ptr<GameScene> prev_;
				boost::shared_ptr<GameScene> next_;
				
				boost::shared_ptr<Scene> scene_;
			private:
				virtual bool init_impl();
				virtual void process()     {}
				virtual void before_draw() {}
				virtual void after_draw()  {}
			protected:
				boost::shared_ptr<Engine>& get_engine() const
				{
					return engine_;
				}
				
				boost::shared_ptr<Scene>& get_scene() const
				{
					return scene_;
				}
				
				const std::string& get_init_arg() const
				{
					return init_arg_;
				}
			public:
				GameScene(boost::shared_ptr<Engine> engine,
						  const std::string& arg = std::string());
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
								
				//	from SceneCallback we derive 2 functions - beforeDraw and
				//	afterDraw
				
				void beforeDraw(Scene* /*scene*/, boost::shared_ptr<iDriver>& /*driver*/)
				{
					process();
					before_draw();
				}
				
				void afterDraw(Scene* /*scene*/, boost::shared_ptr<iDriver>& /*driver*/)
				{
					after_draw();
				}
		};
	}
}

#endif
