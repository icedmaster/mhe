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
		node_.reset(new mhe::game::SpriteComponent("sprite"));
		node_->set_material(mhe::material_ptr(new mhe::Material(
												  engine()->context().texture_manager().get("test_sprite.png"),
												  engine()->context().shader_manager().get("diffuse_unlit"))));
		scene()->add(node_);
		color_animation_.reset(new mhe::game::ColorAnimationComponent(1000, "sprite"));
		color_animation_->set_range(mhe::color_white, mhe::color_black);
		node_->attach(color_animation_);
		
		transform_animation_.reset(new mhe::game::TransformAnimationComponent(1000, "sprite", true));
		transform_animation_->set_range(mhe::matrixf::identity(), mhe::matrixf::translation_matrix(100, 100, 0));
		node_->attach(transform_animation_);

		color_animation_->start();
		transform_animation_->start();

		engine()->component_manager().add(node_);

		init_second_node();
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
		if (!transform_animation_->running())
			transform_animation_->start();
		return true;
	}

	void init_second_node()
	{
		node2_.reset(new mhe::game::SpriteComponent("node2"));
		node2_->set_material(node_->material());
		node2_->translate_to(mhe::vector3<float>(600, 200, 0));
		engine()->component_manager().add(node2_);
		scene()->add(node2_);

		transform_animation2_.reset(new mhe::game::TranslateByAnimationComponent(1000, "node2"));
		transform_animation2_->set_translation(-100, -100, 0);
		node2_->attach(transform_animation2_);
		transform_animation2_->start();
	}

	mhe::utils::Timer timer_;
	boost::shared_ptr<mhe::game::NodeComponent> node_;
	boost::shared_ptr<mhe::game::ColorAnimationComponent> color_animation_;
	boost::shared_ptr<mhe::game::TransformAnimationComponent> transform_animation_;
	boost::shared_ptr<mhe::game::NodeComponent> node2_;
	boost::shared_ptr<mhe::game::TranslateByAnimationComponent> transform_animation2_;
	cmn::uint frames_;
};

int main(int /*argc*/, char** /*argv*/)
{
	mhe::app::Application2D app("06_animation");
	mhe::app::ApplicationConfig config;
	config.width = 800;
	config.height = 600;
	config.bpp = 32;
	config.assets_path = "../../../assets/";
	app.init(config);
	boost::shared_ptr<TestGameScene> game_scene(new TestGameScene(&(app.engine())));
	game_scene->init();
	app.engine().set_game_scene(game_scene);
	game_scene->scene()->add_camera(new mhe::Camera2D(800, 600), true);

	return app.run();
}
