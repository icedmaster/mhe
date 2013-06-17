#include "mhe.hpp"

#include <iostream>

class TestNode : public mhe::Node
{
private:
	void draw_impl(mhe::Context& context)
	{
		glColor4f(1.0, 0.0, 0.0, 1.0);
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(20.0, 0.0, 0.0);
		glEnd();

		glColor4f(0.0, 1.0, 0.0, 1.0);
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 20.0, 0.0);
		glEnd();

		glColor4f(0.0, 0.0, 1.0, 1.0);
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 20.0);
		glEnd();
	}
};

class TestScene : public mhe::game::GameScene
{
public:
	TestScene(mhe::game::Engine* engine) :
		mhe::game::GameScene(engine),
		prev_time_(0)
	{}
private:
	bool init_impl(const std::string&, void*)
	{
		mhe::Camera* camera = new mhe::Camera;
		mhe::matrixf m;
		m.set_perspective(60, 800 / 600, 1, 100);
		camera->set_projection(m);
		m.set_lookAt(mhe::v3d(0, 1, 10), mhe::v3d(), mhe::v3d(0, 1, 0));		
		camera->set_transform(m);
		camera->enable_each_frame_updating(true);
		scene()->add_camera(camera, true);

		get_engine()->font_manager().set_path("../../../assets/fonts/");
		stats_component_.reset(new mhe::game::utils::StatsComponent(scene(), engine()));
		engine()->component_manager().add(stats_component_);

		scene()->add(new TestNode);
		return true;
	}

	bool process_impl()
	{
		cmn::uint now = mhe::utils::get_current_tick();
		if (!prev_time_) prev_time_ = now;
		else if (now >= (prev_time_ + 1000))
		{
			prev_time_ = now;
			std::cout << mhe::utils::Profiler::instance() << std::endl;
			mhe::utils::Profiler::instance().reset();
		}
		return true;
	}

	void draw_impl()
	{
	}

	mhe::game::component_ptr stats_component_;
	cmn::uint prev_time_;
};

int main(int /*argc*/, char** /*argv*/)
{
	mhe::app::Application app("10_3d_basic");
	mhe::app::ApplicationConfig config;
	config.width = 800;
	config.height = 600;
	config.bpp = 32;
	app.init(config);

	// set scene
	boost::shared_ptr<TestScene> scene(new TestScene(&app.engine()));
	app.engine().set_game_scene(scene);
	scene->init("", nullptr);

	app.engine().event_manager().add_device(new mhe::MouseDevice("mouse"));
	app.engine().event_manager().add_device(new mhe::KeyboardDevice("keyb"));

	// run application
	return app.run();
}
