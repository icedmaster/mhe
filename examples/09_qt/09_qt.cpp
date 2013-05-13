#include "mhe.hpp"

#include <iostream>

class TestScene : public mhe::game::GameScene
{
public:
	TestScene(mhe::game::Engine* engine) :
		mhe::game::GameScene(engine)
	{}
private:
	bool init_impl(const std::string&, void*)
	{
		get_engine()->font_manager().set_path("../../../assets/fonts/");
		font_ = get_engine()->font_manager().get("droid-sans.fnt");
		node_.reset(new mhe::gui::GUINode(get_engine()->event_manager()));
		mhe::gui::Widget* widget = new mhe::gui::Widget("main");
		widget->set_sprite(mhe::utils::create_sprite(mhe::color_white, mhe::vector2<float>(200, 200)));
		widget->set_geom(mhe::rect<float>(100, 100, 200, 200));
		node_->add_widget(widget);				
		mhe::gui::Label* label = new mhe::gui::Label("label");
		label->set_sprite(mhe::utils::create_sprite(mhe::color_red, mhe::vector2<float>(64, 64)));
		label->set_geom(mhe::rect<float>(100, 100, 64, 64));
		widget->add_widget(label);
		return true;
	}

	void draw_impl()
	{
		font_->print(get_engine()->context().driver(), mhe::utils::utf8_to_utf32("abcde"),
					 mhe::vector2<float>(200, 200), mhe::color_white);
	}

	boost::shared_ptr<mhe::gui::Font> font_;
	boost::shared_ptr<mhe::gui::GUINode> node_;
};

int main(int /*argc*/, char** argv)
{
	std::cout << mhe::utils::join(mhe::SystemFactory::instance().window_system_factory().available_systems_list()) << std::endl;
	std::cout << "Current:" << mhe::SystemFactory::instance().window_system_factory().current_system_name() << std::endl;
	mhe::app::Application2D application("09_qt");
	mhe::app::ApplicationConfig config;
	config.width = 800;
	config.height = 600;
	config.bpp = 32;
	application.init(config);
	boost::shared_ptr<TestScene> scene(new TestScene(&app.engine()));
	app.engine().set_game_scene(scene);
	scene->init("", nullptr);
	return application.run();
}
