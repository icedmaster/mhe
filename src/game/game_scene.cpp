#include "game/game_scene.hpp"
#include "game/engine.hpp"

namespace mhe {
namespace game {

void GameScene::deinit()
{
	deinit_impl();
	free_resources();
}

void GameScene::free_resources()
{
	engine_->free_all();
}

}}

