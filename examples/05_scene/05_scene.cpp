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
		get_scene()->add_modifier(new mhe::LayerSceneModifier);
		get_scene()->add_modifier(new mhe::BatchSceneModifier);
		// init foreground
		boost::shared_ptr<mhe::Sprite> foreground(
			mhe::utils::create_sprite("../../../assets/star.png",
									  get_engine()->context()));
		foreground->set_priority(mhe::Node::priority_high);
		foreground->translate(400, 300, 0);
		get_scene()->add(foreground);		
		// create 100 sprites
		for (int i = 0; i < 100; ++i)
		{
			const char* filename = !(i % 2) ? "../../../assets/test_sprite.png" :
			  "../../../assets/test_sprite2.png";
			boost::shared_ptr<mhe::Sprite> sprite(
				mhe::utils::create_sprite(filename,
										  get_engine()->context()));
			if (!(i % 4)) sprite->set_priority(mhe::Node::priority_normal + 1);
			get_scene()->add(sprite);
			sprite->translate(mhe::utils::random(800), mhe::utils::random(600), 0);
			sprite->start();
		}
		// init background
		boost::shared_ptr<mhe::Sprite> background(
			mhe::utils::create_sprite("../../../assets/background.png",
									  get_engine()->context()));
		background->set_priority(mhe::Node::priority_low);
		get_scene()->add(background);
		timer_.start();
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
