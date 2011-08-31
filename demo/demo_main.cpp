#include "mhe.hpp"

class MainMenuScene : public mhe::game::GameScene
{
	typedef mhe::PrivateEventListener<MainMenuScene> MainMenuEventListener;
	friend class mhe::PrivateEventListener<MainMenuScene>;
	typedef mhe::gui::PrivateGUIEventHandler<MainMenuScene> MainMenuGUIHandler;
	friend class mhe::gui::PrivateGUIEventHandler<MainMenuScene>;

	boost::shared_ptr<mhe::gui::iFont> fps_font;
private:
	bool init_impl(const std::string& arg)
	{
		load_scene(arg);
		setup_events();
		return true;
	}

	void load_scene(const std::string& arg)
	{
		mhe::mhe_loader loader(arg, &(get_engine()->getTextureManager()),
							   &(get_engine()->getFontManager()));
		loader.load_all_assets();
		boost::shared_ptr<mhe::gui::GUIContainer> gui(loader.getGUI(L"main_menu"));
		gui->setupEvents(get_engine()->getInputSystem());
		setup_gui(gui);
		get_scene()->add(gui);
		boost::shared_ptr<mhe::Sprite> background(loader.getSprite(L"background"));
		background->setPriority(1);
		const mhe::WindowSystem& ws = get_engine()->getWindowSystem();
		background->setSize(ws.width(), ws.height());
		background->execute(0, mhe::utils::get_current_tick());
		//boost::shared_ptr<mhe::Sprite> animated(loader.getSprite(L"animated"));
		get_scene()->add(background);

		fps_font = get_engine()->getFontManager().get("145.fnt");
		boost::shared_ptr<mhe::iNode> fps_node(
			new mhe::utils::GraphicsFPSCounter(get_engine()->getInputSystem(),
											   fps_font, mhe::v2d(20, 550)));
		get_scene()->add(fps_node);
		//get_scene()->add(animated);
	}

	void setup_gui(boost::shared_ptr<mhe::gui::GUIContainer> gui)
	{
		MainMenuGUIHandler* start_move_handler = new MainMenuGUIHandler(this,
																		&MainMenuScene::on_start_mouse_move);
		gui->get("start_button")->setHandler(mhe::gui::OnMouseMove, start_move_handler);
	}

	void setup_events()
	{
		MainMenuEventListener* listener = new MainMenuEventListener(mhe::KeyboardEventType,
																	mhe::KEY_DOWN, SDLK_ESCAPE,
																	this, &MainMenuScene::on_quit);
		get_engine()->getInputSystem().addListener(listener);
	}
private:	// events
	bool on_quit(const mhe::Event&)
	{
		get_engine()->stop();
		return true;
	}

	// gui events
	void on_start_mouse_move(const mhe::gui::Widget*)
	{
		get_engine()->stop();
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
	mhe::utils::global_log::instance().write("Engine stopped");
	return 0;
}
