#include "mhe.hpp"

#include <iostream>

class TestScene : public mhe::game::GameScene
{
public:
	TestScene(mhe::game::Engine* engine) :
		mhe::game::GameScene(engine),
		prev_time_(0)
	{}
private:
	bool init_impl(const mhe::utils::PropertiesList&)
	{		
		node_component_.reset(new mhe::game::SpriteComponent("sprite"));
		node_component_->set_material(engine()->context().material_manager().get_material("logo_0.png", "diffuse_unlit"));

		frame_animation_component_.reset(new mhe::game::FrameAnimationComponent(
											 engine(), "diffuse_unlit", "logo_.png", 5,
											   500, "sprite"));
		scene()->add(node_component_);
		engine()->component_manager().add(node_component_);
		node_component_->attach(frame_animation_component_);
		frame_animation_component_->set_loop(true);
		frame_animation_component_->set_blend_mode(mhe::alpha_one_minus_alpha);
		frame_animation_component_->start();
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
	boost::shared_ptr<mhe::game::NodeComponent> node_component_;
	boost::shared_ptr<mhe::game::FrameAnimationComponent> frame_animation_component_;
};

int main(int /*argc*/, char** /*argv*/)
{
	mhe::app::Application2D app("13_logo");
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
