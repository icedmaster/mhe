#include "mhe.hpp"

class TestScene : public mhe::game::GameScene
{
public:
	TestScene(mhe::game::Engine* engine) :
		mhe::game::GameScene(engine)
	{}
private:
	bool init_impl(const mhe::utils::PropertiesList&)
	{
		engine()->context().texture_manager().set_path("../../../assets/");
		mhe::Sprite* sprite = mhe::utils::create_sprite("test_sprite.png", engine()->context());
		sprite->translate_to(mhe::v3d(200, 200, 0));
		scene()->add(sprite);

		// scaled sprite
		mhe::Sprite* scaled = mhe::utils::create_sprite("test_sprite.png", engine()->context());
		scaled->scale_by(0.5, 0.5, 1);
		scaled->translate_to(mhe::v3d(400, 400, 0));
		scaled->scale_by(0.5, 0.5, 1);
		scene()->add(scaled);
		
		// rotated sprite (rotation CCW)
		mhe::Sprite* rotated = mhe::utils::create_sprite("test_sprite.png", engine()->context());
		rotated->rotate_by(0.0, 0.0, -90.0);
		rotated->translate_to(mhe::v3d(100, 100, 0));
		rotated->rotate_by(0.0, 0.0, 45.0);
		
		scene()->add(rotated);

		// parentness
		mhe::Sprite* parent = mhe::utils::create_sprite("test_sprite.png", engine()->context());
		parent->translate_to(mhe::v3d(400, 100, 0));
		mhe::Sprite* child = mhe::utils::create_sprite("star.png", engine()->context());
		parent->add_node(child);
		scene()->add(parent);
		parent->translate_by(mhe::v3d(50, 0, 0));
		parent->rotate_by(0.0, 0.0, 45);
		parent->scale_to(0.3, 1.2, 1);
		return true;
	}

	void draw_impl()
	{
	}
};

int main(int, char**)
{
	mhe::app::Application2D app("02_node");
	mhe::app::ApplicationConfig config;
	config.width = 800;
	config.height = 600;
	config.bpp = 32;
	config.fullscreen = false;
	app.init(config);
	boost::shared_ptr<TestScene> scene(new TestScene(&app.engine()));
	app.engine().set_game_scene(scene);
	scene->init();

	return app.run();
}
