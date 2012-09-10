#include "mhe.hpp"

class TestGameScene : public mhe::game::GameScene
{
public:
	TestGameScene(mhe::game::Engine* engine) :
		mhe::game::GameScene(engine),
		timer_(1000), frames_(0)
	{}

private:
	bool init_impl(const std::string& /*arg*/, void* /*prm*/)
	{
		// create 100 sprites
		for (int i = 0; i < 100; ++i)
		{
			mhe::AnimationList* al = new mhe::AnimationList(0, true);
			boost::shared_ptr<mhe::Texture> texture =
				get_engine()->context().texture_manager().get("../../../assets/test_sprite.png");
			mhe::Animation* a = new mhe::TextureAnimation(1000, texture);
			al->add(a);
			boost::shared_ptr<mhe::Sprite> sprite(new mhe::Sprite(al));
			get_scene()->add(sprite);
			sprite->translate(mhe::utils::random(800), mhe::utils::random(600), 0);
			sprite->start();
		}
		timer_.start();
		return true;
	}

	bool process_impl()
	{
		if (timer_.elapsed())
		{
			std::cout << "fps:" << frames_ << "\n";
			frames_ = 0;
			timer_.start();
		}
		else ++frames_;
	}

	mhe::utils::Timer timer_;
	cmn::uint frames_;
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
