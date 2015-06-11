#include "mhe.hpp"

class GameScene : public mhe::game::GameScene
{
public:
	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/) override
	{
		imgui_.init(&engine);
		v_[0] = 0.0; v_[1] = 1.0f; v_[2] = 2.0;
		return true;
	}

	bool update(mhe::game::Engine& engine) override
	{
		imgui_.update(engine.context(), engine.render_context(), engine.event_manager());

		ImGui::Begin("mhe");
		ImGui::LabelText("Lbl:", "This is a test label");
		ImGui::SliderFloat3("And this is a test slider", v_, 0.0f, 10.0f);
		ImGui::End();

		return true;
	}

	void before_draw(mhe::game::Engine& engine) override
	{
		imgui_.render(engine.context(), engine.render_context());
	}
private:
	mhe::ImGuiHelper imgui_;
	float v_[3];
};

int main(int /*argc*/, char** /*argv*/)
{
	mhe::app::Application app("10_imgui");
	mhe::app::ApplicationConfig config;
	config.width = 1280;
	config.height = 720;
	config.bpp = 32;
	config.fullscreen = false;
#ifndef MHE_VS
	config.assets_path = "../assets/";
#else
	config.assets_path = "../../assets/";
#endif
	config.render_config_filename = mhe::utils::path_join(config.assets_path, "render.xml");
	app.init(config);

	mhe::game::GameSceneDesc desc;
	GameScene* game_scene = new GameScene;
	game_scene->init(app.engine(), desc);
	app.engine().set_game_scene(game_scene);

	return app.run();
}
