#include "game_scene.hpp"

namespace mhe  {
namespace game {

	GameScene::GameScene(boost::shared_ptr<Engine> engine,
						 const std::string& arg = std::string()) :
		engine_(engine),
		init_arg_(arg)
	{
	}
	
	bool GameScene::init_impl()
	{
		scene_.reset(new Scene());
		scene->setCallback(this);
		engine_->setScene(scene_);
		return true;
	}
	
}
}
