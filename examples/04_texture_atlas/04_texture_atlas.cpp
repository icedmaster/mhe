#include "mhe.hpp"

class TestGameScene : public mhe::game::GameScene
{
public:
	TestGameScene(mhe::game::Engine* engine) :
		mhe::game::GameScene(engine)
	{}

private:
	bool init_impl(const std::string& /*arg*/, void* /*prm*/)
	{
		const boost::shared_ptr<mhe::TextureAtlas>& atlas = 
			get_engine()->context().texture_atlas_manager().get("../../../assets/test_atlas.atlas");
		mhe::TextureAnimation* animation = new mhe::TextureAnimation(1000, atlas->texture());
		animation->set_texcoord(mhe::vector_to_texcoord_container(atlas->get("test_sprite.png")));		
		mhe::AnimationList* al1 = new mhe::AnimationList(0);
		al1->add(animation);
		boost::shared_ptr<mhe::Sprite> sprite1(new mhe::Sprite(al1));
		const mhe::rect<float>& rect = atlas->get_rect("test_sprite.png");
		sprite1->set_size(rect.width(), rect.height());
		scene()->add(sprite1);
		sprite1->start();
		return true;
	}
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
	engine.context().driver().set_projection_matrix(proj);
	boost::shared_ptr<TestGameScene> game_scene(new TestGameScene(&engine));
	game_scene->init("", nullptr);
	engine.set_game_scene(game_scene);
	engine.run();

	mhe::impl::stop_platform();

	return 0;
}
