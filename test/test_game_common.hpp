#ifndef __TEST_GAME_COMMON_HPP__
#define __TEST_GAME_COMMON_HPP__

#include "test_common.hpp"

namespace mhe {
namespace test {

	class TestGameCommon : public ::testing::Test
	{
		protected:
			class TestGameScene : public game::GameScene
			{
			public:
				TestGameScene(game::Engine* engine) : game::GameScene(engine) {}
			};
		protected:
			game::Engine engine; 	
		protected:
			virtual void SetUp()
			{
				engine.init(800, 600, 32, false);
		
				mhe::matrixf proj;
				proj.set_ortho(0, 800, 0, 600, -1, 1);
				engine.getDriver()->set_projection_matrix(proj);
				engine.setGameScene(boost::shared_ptr<TestGameScene>(new TestGameScene(&engine)));
			}
	};
	
}
}

#endif
