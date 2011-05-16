#include "game_scene.hpp"

namespace game
{
	GameScene::GameScene()
	{}
	
	GameScene::GameScene(const GameScene&)
	{}
	
	bool GameScene::init(cmn::uint w, cmn::uint h, cmn::uint bpp)
	{
		if (ws_) return true;	// already initialized
		return init_impl(w, h, bpp);
	}
	
	bool GameScene::init_impl(cmn::uint w, cmn::uint h, cmn::uint bpp)
	{
		ws_.reset(new mhe::WindowSystem());
		is_.reset(new mhe::InputSystem());
		driver_.reset(new mhe::OpenGLDriver());
		driver->setWindowSystem(ws_.get());
		tm_.reset(new mhe::TextureManager());
		
		mhe::load_default_extensions();
		
		if (!ws_.init(w, h, bpp))
			return false;
		return true;
	}
	
	GameScene* GameScene::clone() const
	{
		GameScene* gs = new GameScene();
		gs->ws_ = ws_;
		gs_->is_ = is_;
		gs_->driver_ = driver_;
		gs_->tm_ = tm_;
		
		return gs;
	}
}
