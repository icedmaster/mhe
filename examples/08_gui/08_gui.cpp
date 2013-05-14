#include "mhe.hpp"

#include "platform/opengl/mhe_gl.hpp"

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
		widget->set_sprite(mhe::utils::create_sprite(mhe::color_white, mhe::vector2<float>(32, 32), get_engine()->context()));
		widget->set_geom(mhe::rect<float>(100, 100, 200, 100));
		node_->set_widget(widget);				
		mhe::gui::Label* label = new mhe::gui::Label("label");
		label->set_sprite(mhe::utils::create_sprite(mhe::color_red, mhe::vector2<float>(32, 32), get_engine()->context()));
		label->set_geom(mhe::rect<float>(10, 10, 50, 20));
		label->set_font(font_);
		label->set_caption(mhe::utils::utf8_to_utf32("label"));
		label->set_caption_color(mhe::color_blue);
		widget->add_widget(label);
		// button
		mhe::gui::Button* button = new mhe::gui::Button("button");
		button->set_sprite(mhe::utils::create_sprite(mhe::color_green, mhe::vector2<float>(32, 32), get_engine()->context()));
		button->set_pressed_sprite(mhe::utils::create_sprite(mhe::color_yellow, mhe::vector2<float>(32, 32), get_engine()->context()));
		button->set_geom(mhe::rect<float>(50, 50, 50, 30));
		widget->add_widget(button);
		scene()->add(node_);
		return true;
	}

	void draw_impl()
	{
	}

	boost::shared_ptr<mhe::gui::Font> font_;
	boost::shared_ptr<mhe::gui::GUINode> node_;
};

int main(int, char**)
{
	mhe::app::Application2D app("gui_test");
	mhe::app::ApplicationConfig config;
	config.width = 800;
	config.height = 600;
	config.bpp = 32;
	config.fullscreen = false;
	app.init(config);
	boost::shared_ptr<TestScene> scene(new TestScene(&app.engine()));
	app.engine().set_game_scene(scene);
	scene->init("", nullptr);

	app.engine().event_manager().add_device(new mhe::MouseDevice("mouse"));
	
	return app.run();
}
