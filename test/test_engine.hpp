#ifndef __TEST_ENGINE_HPP__
#define __TEST_ENGINE_HPP__

#include "test_common.hpp"
#include "game/engine.hpp"

TEST(EngineTest, base_test)
{
	mhe::game::Engine engine;
	ASSERT_TRUE(engine.init(800, 600, 32));
	engine.run();
}

#endif

