#ifndef _GAME_SCENE_HPP_
#define _GAME_SCENE_HPP_

#include <boost/shared_ptr.hpp>
#include "mhe.hpp"

namespace game
{
	class GameScene
	{
		private:
			boost::shared_ptr<GameScene> prev_;
			boost::shared_ptr<GameScene> next_;
			
			// engine entities
			boost::shared_ptr<mhe::WindowSystem> ws_;
			boost::shared_ptr<mhe::InputSystem> is_;
			boost::shared_ptr<mhe::iDriver> driver_;
			boost::shared_ptr<mhe::TextureManager> tm_;
			
			void operator= (const GameScene&) {}
		public:
			GameScene();
			GameScene(const GameScene&);
			
			bool init(cmn::uint w, cmn::uint h, cmn::uint bpp);
			
			GameScene* clone() const;
	};
}

#endif
