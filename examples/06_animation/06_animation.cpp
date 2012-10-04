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
		if (!change_type == mhe::AnimationListBase::last_animation) return;
		set_next_animation();
	}
private:
	bool init_impl(const std::string& /*arg*/, void* /*prm*/)
	{
		sprite_.reset(new mhe::Sprite);
		sprite_->set_texture(get_engine()->context().texture_manager().get("../../../assets/test_sprite.png"));
		add_linear_color_animation();
		add_position_animation();
		get_scene()->add(sprite_);
		sprite_->current_animation_list()->add_delegate(mhe::create_delegate(this, &TestGameScene::on_animation_end));
		return true;
	}

	bool process_impl()
	{
		if (timer_.elapsed())
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
	}

	void add_position_animation()
	{
		mhe::TransformLinearAnimationList* al = new mhe::TransformLinearAnimationList(1);
		al->set_translate_animation(mhe::v3d(300, 300, 0), 100, 2000);
		sprite_->add_animation_list(al);
	}

	void set_next_animation()
	{
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
	mhe::utils::create_standart_log();
	mhe::utils::init_randomizer();
	mhe::impl::start_platform();
	mhe::game::Engine engine;
	if (!engine.init(800, 600, 32))
	{
		std::cerr << "Can't init engine\n";
		return 1;
	}
	std::cout << "Engine initialized sucessfully\n";
	mhe::matrixf proj;
	proj.set_ortho(0, 800, 0, 600, -1, 1);
	engine.context().driver()->set_projection_matrix(proj);
	boost::shared_ptr<TestGameScene> game_scene(new TestGameScene(&engine));
	game_scene->init("", nullptr);
	engine.set_game_scene(game_scene);
	engine.run();

	mhe::impl::stop_platform();

	return 0;
}
