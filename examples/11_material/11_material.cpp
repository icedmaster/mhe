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
		const mhe::KeyboardEvent* ke = static_cast<const mhe::KeyboardEvent*>(event);
		if (ke->sym() == mhe::KeyboardDevice::key_1)
			sprite_->set_material(materials_[0]);
		else if (ke->sym() == mhe::KeyboardDevice::key_2)
			sprite_->set_material(materials_[1]);
		return true;
	}
private:
	bool init_impl(const mhe::utils::PropertiesList&)
	{		
		sprite_.reset(new mhe::Sprite);
		materials_[0] = engine()->context().material_manager().get_material("test_sprite.png", "diffuse_unlit");
		materials_[1] = engine()->context().material_manager().get_material("test_sprite.png", "diffuse_unlit_multitexture");
		materials_[1]->add_texture(engine()->context().texture_manager().get("highlight.png"));
		scene()->add(sprite_);
		sprite_->set_material(materials_[1]);

		engine()->event_manager().add_listener(new mhe::DelegateEventListener(mhe::keyboard_event_type,
																			  mhe::KeyboardEvent::key_down, mhe::Event::any_event,
																			  mhe::create_delegate(this, &TestScene::process_key)));
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
	mhe::material_ptr materials_[2];
	boost::shared_ptr<mhe::Sprite> sprite_;
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
