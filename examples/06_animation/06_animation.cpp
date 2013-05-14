#include "mhe.hpp"

class TestGameScene : public mhe::game::GameScene
{
public:
	TestGameScene(mhe::game::Engine* engine) :
		mhe::game::GameScene(engine),
		timer_(1000), frames_(0),
		current_animation_(0)
	{}

	void on_animation_end(mhe::AnimationListBase* /*animation_list*/,
						  mhe::AnimationListBase::AnimationChangeType change_type)
	{
		if (change_type != mhe::AnimationListBase::last_animation) return;
		DEBUG_LOG("animation end:" << change_type);
		set_next_animation();
	}
private:
	bool init_impl(const std::string& /*arg*/, void* /*prm*/)
	{
		sprite_.reset(new mhe::Sprite);
		sprite_->set_texture(get_engine()->context().texture_manager().get("../../../assets/test_sprite.png"));
		add_linear_color_animation();
		add_position_animation();
		scene()->add(sprite_);
		timer_.start();
		set_next_animation();
		return true;
	}

	bool process_impl()
	{
		if (timer_.elapsed() && frames_)
		{
			std::cout << "fps:" << frames_ << " tris:" <<
				get_engine()->context().driver()->stats().tris() / frames_ <<
				" dips:" << get_engine()->context().driver()->stats().batches() / frames_ << "\n";
			get_engine()->context().driver()->stats().reset();
			frames_ = 0;
			timer_.start();
		}
		else ++frames_;
		return true;
	}

	void add_linear_color_animation()
	{
		mhe::ColorLinearAnimationList* al = new mhe::ColorLinearAnimationList(0);
		al->set_range(mhe::color_white, mhe::color_black, 100, 2000);
		sprite_->add_animation_list(al);
		al->add_delegate(mhe::create_delegate(this, &TestGameScene::on_animation_end));
	}

	void add_position_animation()
	{
		mhe::TransformLinearAnimationList* al = new mhe::TransformLinearAnimationList(1);
		al->set_translate_animation(mhe::v3d(300, 300, 0), 100, 2000);
		sprite_->add_animation_list(al);
		al->add_delegate(mhe::create_delegate(this, &TestGameScene::on_animation_end));
	}

	void set_next_animation()
	{
		sprite_->set_color(mhe::color_white);
		sprite_->identity();
		sprite_->execute(current_animation_++);
		if (current_animation_ >= 2)
			current_animation_ = 0;
	}

	mhe::utils::Timer timer_;
	cmn::uint frames_;
	boost::shared_ptr<mhe::Sprite> sprite_;
	size_t current_animation_;
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
	game_scene->init("", nullptr);
	app.engine().set_game_scene(game_scene);
	game_scene->scene()->add_camera(new mhe::Camera2D(800, 600), true);

	return app.run();
}
