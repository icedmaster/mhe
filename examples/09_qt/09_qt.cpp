#include "mhe.hpp"

#include <iostream>
#include <QWidget>
#include <QPushButton>

class TestScene : public mhe::game::GameScene
{
public:
	TestScene(mhe::game::Engine* engine) :
		mhe::game::GameScene(engine)
	{}

	bool handle_key(const mhe::Event* event)
	{
		const mhe::KeyboardEvent* ke = static_cast<const mhe::KeyboardEvent*>(event);
		std::cout << "key pressed:" << ke->state() << " " << ke->sym() << std::endl;
		return true;
	}
private:
	bool init_impl(const std::string&, void*)
	{
		get_engine()->font_manager().set_path("../../../assets/fonts/");
		font_ = get_engine()->font_manager().get("droid-sans.fnt");
		node_.reset(new mhe::gui::GUINode(get_engine()->event_manager()));
		mhe::gui::Widget* widget = new mhe::gui::Widget("main");
		widget->set_sprite(mhe::utils::create_sprite(mhe::color_white, mhe::vector2<float>(200, 200), get_engine()->context()));
		widget->set_geom(mhe::rect<float>(100, 100, 200, 100));
		node_->set_widget(widget);				
		mhe::gui::Label* label = new mhe::gui::Label("label");
		label->set_sprite(mhe::utils::create_sprite(mhe::color_red, mhe::vector2<float>(64, 64), get_engine()->context()));
		label->set_geom(mhe::rect<float>(10, 10, 50, 20));
		label->set_font(font_);
		label->set_caption(mhe::utils::utf8_to_utf32("label"));
		widget->add_widget(label);
		// button
		mhe::gui::Button* button = new mhe::gui::Button("button");
		button->set_sprite(mhe::utils::create_sprite(mhe::color_green, mhe::vector2<float>(32, 32), get_engine()->context()));
		button->set_pressed_sprite(mhe::utils::create_sprite(mhe::color_yellow, mhe::vector2<float>(32, 32), get_engine()->context()));
		button->set_geom(mhe::rect<float>(50, 50, 50, 30));
		widget->add_widget(button);

		scene()->add(node_);

		engine()->event_manager().add_listener(new mhe::DelegateEventListener(mhe::keyboard_event_type,
																			  mhe::Event::any_event, mhe::Event::any_event,
																			  mhe::create_delegate(this, &TestScene::handle_key)));

		stats_component_.reset(new mhe::game::utils::StatsComponent(scene(), engine()));
		engine()->component_manager().add(stats_component_);
		return true;
	}

	void draw_impl()
	{
	}

	boost::shared_ptr<mhe::gui::Font> font_;
	boost::shared_ptr<mhe::gui::GUINode> node_;
	mhe::game::component_ptr stats_component_;
};

int main(int argc, char** argv)
{
	// 1) setup Qt application - need to do it before first widget will be created
	QApplication qapp(argc, argv);
	// 2) check we using Qt now
	std::cout << mhe::utils::join(mhe::SystemFactory::instance().window_system_factory().available_systems_list(), ",") << std::endl;
	std::cout << "Current:" << mhe::SystemFactory::instance().window_system_factory().current_system_name() << std::endl;
	// 3) setup GUI
	QWidget* widget = new QWidget;
	widget->setGeometry(0, 0, 800, 600);
	mhe::qt::QtView* view = new mhe::qt::QtView(widget);
	view->setGeometry(100, 100, 400, 400);
	QPushButton* button = new QPushButton(widget);
	button->setGeometry(10, 500, 50, 20);

	// 4) setup mhe application
	mhe::app::Application2D app("09_qt");
	// set view BEFORE app.init() call
	app.engine().context().window_system().set_view(view);
	// configure view - size the same as view geometry size
	mhe::app::ApplicationConfig config;
	config.width = 400;
	config.height = 400;
	config.bpp = 32;
	app.init(config);

	// set scene
	boost::shared_ptr<TestScene> scene(new TestScene(&app.engine()));
	app.engine().set_game_scene(scene);
	scene->init("", nullptr);

	app.engine().event_manager().add_device(new mhe::MouseDevice("mouse"));
	app.engine().event_manager().add_device(new mhe::KeyboardDevice("keyb"));

	// show our GUI
	widget->show();
	view->setFocus();

	// run application
	return app.run();
}
