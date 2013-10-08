#include "mhe.hpp"

class TestGameScene : public mhe::game::GameScene
{
public:
	TestGameScene(mhe::game::Engine* engine) :
		mhe::game::GameScene(engine),
		timer_(1000), frames_(0)
	{}
private:
	bool init_impl(const mhe::utils::PropertiesList&)
	{
		engine()->context().texture_manager().set_path("../../../assets/");
		mhe::Sprite* sprite = new mhe::Sprite;
		sprite->set_texture(engine()->context().texture_manager().get("test_sprite.png"));
		mhe::game::NodeComponent* node = new mhe::game::NodeComponent("sprite", "node", sprite, scene());
		color_animation_.reset(new mhe::game::ColorAnimationComponent(1000, "sprite", "animation"));
		color_animation_->set_range(mhe::color_white, mhe::color_black);
		node->attach(color_animation_);
		color_animation_->start();
		engine()->component_manager().add(node);
		timer_.start();
		return true;
	}

	bool process_impl()
	{
		if (timer_.elapsed() && frames_)
		{
			std::cout << "fps:" << frames_ << " tris:" <<
				get_engine()->context().driver().stats().tris() / frames_ <<
				" dips:" << get_engine()->context().driver().stats().batches() / frames_ << "\n";
			get_engine()->context().driver().stats().reset();
			frames_ = 0;
			timer_.start();
		}
		else ++frames_;
		if (!color_animation_->running())
			color_animation_->start();
		return true;
	}

	mhe::utils::Timer timer_;
	boost::shared_ptr<mhe::game::ColorAnimationComponent> color_animation_;
	cmn::uint frames_;
};

int main(int /*argc*/, char** /*argv*/)
{
	mhe::app::Application2D app("06_animation");
	mhe::app::ApplicationConfig config;
	config.width = 800;
	config.height = 600;
	config.bpp = 32;
	config.assets_path = "../../../assets";
	app.init(config);
	boost::shared_ptr<TestGameScene> game_scene(new TestGameScene(&(app.engine())));
	game_scene->init();
	app.engine().set_game_scene(game_scene);
	game_scene->scene()->add_camera(new mhe::Camera2D(800, 600), true);

	return app.run();
}
