#include "mhe.hpp"

#include <iostream>

class TestScene : public mhe::game::GameScene
{
public:
	TestScene(mhe::game::Engine* engine) :
		mhe::game::GameScene(engine),
		prev_time_(0)
	{}

	bool process_key(const mhe::Event* event)
	{
		return true;
	}
private:
	bool init_impl(const mhe::utils::PropertiesList&)
	{		
		mhe::Sprite* sprite = new mhe::Sprite;
		sprite->set_material(engine()->context().material_manager().get_material("test_sprite.png", "diffuse_unlit"));
		scene()->add(sprite);
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

	cmn::uint prev_time_;
	mhe::material_ptr material_;

};

int main(int /*argc*/, char** /*argv*/)
{
	mhe::app::Application2D app("10_3d_basic");
	mhe::app::ApplicationConfig config;
	config.width = 800;
	config.height = 600;
	config.bpp = 32;
	config.fullscreen = false;
	config.assets_path = "../../../assets/";
	app.init(config);

	// set scene
	boost::shared_ptr<TestScene> scene(new TestScene(&app.engine()));
	app.engine().set_game_scene(scene);

	app.engine().event_manager().add_device(new mhe::MouseDevice("mouse"));
	app.engine().event_manager().add_device(new mhe::KeyboardDevice("keyb"));

	scene->init();

	// run application
	return app.run();
}
