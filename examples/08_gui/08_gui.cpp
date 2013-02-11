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
		return true;
	}

	void draw_impl()
	{
		font_->print(get_engine()->context().driver(), mhe::utils::utf8_to_utf32("abcde"),
					 mhe::vector2<float>(200, 200), mhe::color_white);
	}

	boost::shared_ptr<mhe::gui::Font> font_;
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
	
	return app.run();
}
