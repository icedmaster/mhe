#ifndef __TEST_SCENE_HPP__
#define __TEST_SCENE_HPP__

#include "window_system.hpp"
#include "inode.hpp"
#include "scene.hpp"
#include "test_common.hpp"

class TestScene : public mhe::test::TestCommon
{
	protected:
		void SetUp()
		{
			mhe::test::TestCommon::SetUp();
			// create 2dcamera
			camera.reset(new mhe::Camera());
			mhe::matrixf om;
			om.set_ortho(0, 800, 0, 600, -1, 1);
			camera->set_transform(om);
			camera->set_id(1);
			scene.addCamera(camera);
		}
	mhe::Scene scene;
	boost::shared_ptr<mhe::iCamera> camera;
};

TEST_F(TestScene, base_test)
{
	// simply add node
	boost::shared_ptr<mhe::iNode> axis_node(new mhe::test::Axis);
	scene.add(axis_node);
	while (running)
	{
		input_system.check();
		driver->clear_depth();
		driver->clear_color();
		mhe::matrixf om;
		om.set_ortho(0, 800, 0, 600, -1, 1);
		driver->set_projection_matrix(om);
		driver->set_modelview_matrix(mhe::matrixf::identity());
		scene.draw(driver);
		window_system.swapBuffers();
	}
}

#endif
