#include "mhe.hpp"

class MainMenuScene : public mhe::game::GameScene
{
	typedef mhe::PrivateEventListener<MainMenuScene> MainMenuEventListener;
	friend class mhe::PrivateEventListener<MainMenuScene>;
	typedef mhe::gui::PrivateGUIEventHandler<MainMenuScene> MainMenuGUIHandler;
	friend class mhe::gui::PrivateGUIEventHandler<MainMenuScene>;

	boost::shared_ptr<mhe::gui::iFont> fps_font;
	boost::shared_ptr<mhe::game::HLWidget> hl_start_button;
	boost::shared_ptr<mhe::game::HLWidget> hl_quit_button;
	boost::shared_ptr<mhe::iSound> theme_sound;
	cmn::uint loop_wait_time;
private:
	bool init_impl(const std::string& arg)
	{
		load_scene(arg);
		setup_events();
		// begin to play beautiful music
		theme_sound = get_engine()->getSoundManager().get("theme");
		theme_sound->play();
		return true;
	}

	void load_scene(const std::string& arg)
	{
		mhe::mhe_loader loader(arg, &(get_engine()->getTextureManager()),
							   &(get_engine()->getFontManager()),
							   &(get_engine()->getSoundManager()));
		loader.load_all_assets();
		boost::shared_ptr<mhe::gui::GUIContainer> gui(loader.getGUI(L"main_menu"));
		gui->setupEvents(get_engine()->getInputSystem());
		setup_gui(gui);
		get_scene()->add(gui);
		boost::shared_ptr<mhe::Sprite> background(loader.getSprite(L"background"));
		background->setPriority(1);
		const mhe::WindowSystem& ws = get_engine()->getWindowSystem();
		background->setSize(ws.width(), ws.height());
		background->start(0);
		//boost::shared_ptr<mhe::Sprite> animated(loader.getSprite(L"animated"));
		get_scene()->add(background);

		fps_font = get_engine()->getFontManager().get("145.fnt");
		boost::shared_ptr<mhe::iNode> fps_node(
			new mhe::utils::GraphicsFPSCounter(get_engine()->getInputSystem(),
											   fps_font, mhe::v2d(20, 550)));
		get_scene()->add(fps_node);
		//get_scene()->add(animated);
	}

	void setup_gui(boost::shared_ptr<mhe::gui::GUIContainer> guic)
	{
		MainMenuGUIHandler* quit_click_handler = new MainMenuGUIHandler(this,
		 																&MainMenuScene::on_quit_mouse_click);
		boost::shared_ptr<mhe::gui::Widget> start_button = guic->get("start_button");
		hl_start_button.reset(new mhe::game::HLWidget(start_button));
													  
		// start button
		MainMenuGUIHandler* start_click_handler = new MainMenuGUIHandler(this,
		 																&MainMenuScene::on_start_mouse_click);
		start_button->set_handler(mhe::gui::OnMouseLeftClick, start_click_handler);
		// quit button
		/*boost::shared_ptr<mhe::gui::Widget> quit_button = guic->get("quit_button");
		quit_button->setHandler(mhe::gui::OnMouseLeftClick, quit_click_handler);
		hl_quit_button.reset(new mhe::game::HLWidget(quit_button,
		get_engine()->getTextureManager().get("hl_quit_button")));*/
	}

	void setup_events()
	{
		MainMenuEventListener* listener = new MainMenuEventListener(mhe::KeyboardEventType,
																	mhe::KEY_DOWN, SDLK_ESCAPE,
																	this, &MainMenuScene::on_quit);
		get_engine()->getInputSystem().addListener(listener);
	}

	// main process function
	bool process_impl()
	{
		if (!theme_sound->is_playing())
		{
			// wait 2 seconds and play it again
			cmn::uint tick = mhe::utils::get_current_tick();
			if (!loop_wait_time)
				loop_wait_time = tick + 2000;
			else if (tick >= loop_wait_time)
			{
				theme_sound->play();
				loop_wait_time = 0;
			}
		}
		return true;
	}
private:	// events
	bool on_quit(const mhe::Event&)
	{
		get_engine()->stop();
		return true;
	}

	// gui events
	void on_quit_mouse_click(const mhe::gui::Widget*)
	{
		get_engine()->stop();
	}

	void on_start_mouse_click(const mhe::gui::Widget*)
	{
		get_engine()->getSoundManager().get("click")->play();
		// create fade effect
		boost::shared_ptr<mhe::game::FadeSprite> fade(
			new mhe::game::FadeSprite(mhe::rect<float>(0.0, 0.0, 
													   get_engine()->getWindowSystem().width(),
													   get_engine()->getWindowSystem().height()),
									  1000));
		get_scene()->add(fade);
		get_engine()->getInputSystem().disable_input();
	}
public:
	MainMenuScene(mhe::game::Engine* engine) : mhe::game::GameScene(engine),
											   loop_wait_time(0)
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
