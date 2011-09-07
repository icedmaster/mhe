#ifndef __TEST_ASPECT_HPP__
#define __TEST_APSECT_HPP__

#include "test_common.hpp"
#include "mhe_game.hpp"

class TestAspect : public mhe::test::TestCommon
{
protected:
	mhe::game::Aspect* base;
	boost::shared_ptr<mhe::iNode> base_node;
	void SetUp()
	{
		mhe::test::TestCommon::SetUp();
		base_node.reset(new mhe::test::Quad);
		dynamic_cast<mhe::test::Quad*>(base_node.get())->set_position(mhe::v3d(200, 200, 0));
	}
};

TEST_F(TestAspect, test_base)
{
	base = new mhe::game::GraphicsAspect(base_node);

	while (running)
	{
		input_system.check();
		driver->clear_depth();
		driver->clear_color();		
		mhe::game::GraphicsAspectParam prm;
		prm.driver = driver;
		base->update(mhe::game::Aspect::DrawEvent, mhe::utils::get_current_tick(), prm);
		window_system.swapBuffers();
	}
}

#endif
