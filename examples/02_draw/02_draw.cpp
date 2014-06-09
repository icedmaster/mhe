#include "mhe.hpp"

class GameScene : public mhe::game::GameScene
{
public:
	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/)
	{
		mhe::Mesh mesh;
		mhe::utils::create_plane(mesh, engine.context());
		return true;
	}

	bool update(mhe::game::Engine& engine)
	{
		return true;
	}
};

int main(int argc, char** argv)
{
	mhe::app::Application app("02_draw");
	mhe::app::ApplicationConfig config;
	config.width = 800;
	config.height = 600;
	config.bpp = 32;
	config.fullscreen = false;
	app.init(config);
	mhe::game::GameSceneDesc desc;
	GameScene* game_scene = new GameScene;
	game_scene->init(app.engine(), desc);
	app.engine().set_game_scene(game_scene);
	return app.run();
}
