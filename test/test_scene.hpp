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
	dynamic_cast<mhe::test::Axis*>(axis_node.get())->set_center(mhe::v3d(400, 300, 0));
	scene.add(axis_node);

	boost::shared_ptr<mhe::test::Quad> quad1(new mhe::test::Quad);
	quad1->set_position(mhe::v3d(100, 100, 0));
	scene.add(quad1);
	while (running)
	{
		input_system.check();
		driver->clear_depth();
		driver->clear_color();
		driver->set_modelview_matrix(mhe::matrixf::identity());
		scene.draw(driver);
		window_system.swapBuffers();
	}
}

// You must see 3 rectangles: part of red, then part of white, then part of green
TEST_F(TestScene, priority_test)
{
	boost::shared_ptr<mhe::test::Quad> quad1(new mhe::test::Quad);
	quad1->set_position(mhe::v3d(400, 300, 0));
	boost::shared_ptr<mhe::test::Quad> quad2(new mhe::test::Quad);
	quad2->set_position(mhe::v3d(370, 300, 0));
	quad2->set_color(mhe::cfRed);
	quad2->setPriority(1);
	boost::shared_ptr<mhe::test::Quad> quad3(new mhe::test::Quad);
	quad3->set_position(mhe::v3d(440, 300, 0));
	quad3->set_color(mhe::cfGreen);
	quad3->setPriority(3);
	scene.add(quad1);
	scene.add(quad2);
	scene.add(quad3);
	while (running)
	{
		input_system.check();
		driver->clear_depth();
		driver->clear_color();
		driver->set_modelview_matrix(mhe::matrixf::identity());
		scene.draw(driver);
		window_system.swapBuffers();
	}
}

//
TEST_F(TestScene, subscene_test)
{
	boost::shared_ptr<mhe::Scene> subscene(new mhe::Scene);
	boost::shared_ptr<mhe::test::Quad> quad1(new mhe::test::Quad);
	quad1->set_position(mhe::v3d(400, 300, 0));
	boost::shared_ptr<mhe::test::Quad> quad2(new mhe::test::Quad);
	quad2->set_position(mhe::v3d(200, 300, 0));
	quad2->set_color(mhe::cfRed);
	subscene->add(quad2);
	scene.add(quad1);
	scene.addSubscene(subscene);
	while (running)
	{
		input_system.check();
		driver->clear_depth();
		driver->clear_color();
		driver->set_modelview_matrix(mhe::matrixf::identity());
		scene.draw(driver);
		window_system.swapBuffers();
	}
}

#endif
