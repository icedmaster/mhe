#include "mhe.hpp"
#include "test/test_common.hpp"

class MainMenuScene : public mhe::game::GameScene
{
private:
	bool init_impl(const std::string& arg)
	{
		boost::shared_ptr<mhe::test::Quad> quad(new mhe::test::Quad);
		quad->set_position(mhe::v3d(100, 100, 0));
		get_scene()->add(quad);

		load_scene(arg);
		return true;
	}

	void load_scene(const std::string& arg)
	{
		mhe::mhe_loader loader(arg, &(get_engine()->getTextureManager()),
							   &(get_engine()->getFontManager()));
		boost::shared_ptr<mhe::gui::GUIContainer> gui(loader.getGUI(L"main_menu"));
		gui->setupEvents(get_engine()->getInputSystem());
		get_scene()->add(gui);
		boost::shared_ptr<mhe::Sprite> background(loader.getSprite(L"background"));
		background->setPriority(1);
		const mhe::WindowSystem& ws = get_engine()->getWindowSystem();
		background->setSize(ws.width(), ws.height());
		background->execute(0, mhe::utils::get_current_tick());
		//boost::shared_ptr<mhe::Sprite> animated(loader.getSprite(L"animated"));
		get_scene()->add(background);
		//get_scene()->add(animated);
	}
public:
	MainMenuScene(mhe::game::Engine* engine) : mhe::game::GameScene(engine)
	{}
};

int main(int argc, char** argv)
{
	mhe::utils::createStandartLog();
    mhe::utils::init_randomizer();

	mhe::game::Engine engine;
	if (!engine.init(800, 600, 32))
	{
		mhe::utils::global_log::instance().write("engine init failed");
		return 1;
	}
	mhe::utils::global_log::instance().write("Engine successfully initialized");

	// setup ortho projection by default
	mhe::matrixf proj;
	proj.set_ortho(0, 800, 0, 600, -1, 1);
	engine.getDriver()->set_projection_matrix(proj);

	boost::shared_ptr<MainMenuScene> scene(new MainMenuScene(&engine));
	if (!scene->init("assets/main_menu.mhe"))
	{
		mhe::utils::global_log::instance().write("Scene init failed");
		return 1;
	}

	engine.setGameScene(scene);
	engine.getDriver()->disable_lighting();

	engine.run();
	return 0;
}
