#include "mhe.hpp"

class GameScene : public mhe::game::GameScene
{
public:
	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/) override
	{
		mhe::TextureInstance texture;
		engine.context().texture_manager.get(texture, mhe::string("checker.tga"));
		texture_ = &engine.context().texture_pool.get(texture.id);

		mhe::TextureDesc desc;
		desc.address_mode_s = mhe::texture_clamp;
		desc.address_mode_t = mhe::texture_clamp;
		desc.format = mhe::format_rgba;
		desc.datatype = mhe::format_default;
		desc.height = 1024;
		desc.width = 1024;
		desc.mag_filter = mhe::texture_filter_linear;
		desc.min_filter = mhe::texture_filter_linear;
		desc.mips = 1;
		desc.type = mhe::texture_2d;
		
		mhe::Texture& blurred_texture = create_and_get(engine.context().texture_pool);
		if (!blurred_texture.init(desc, nullptr, 0))
			return false;
		blurred_texture_ = &blurred_texture;

		mhe::PosteffectDebugMaterialSystem* material_system = engine.context().material_systems.get<mhe::PosteffectDebugMaterialSystem>();
		mhe::TextureInstance blurred_texture_instance;
		blurred_texture_instance.id = blurred_texture.id();
		material_system->set_texture(0, blurred_texture_instance);
		material_system->enable();

		mhe::Shader shader;
		engine.context().shader_manager.get(shader, mhe::string("13_compute"));
		shader_program_ = &engine.context().shader_pool.get(engine.context().ubershader_pool.get(shader.shader_program_handle).get_default());
		return true;
	}

	bool update(mhe::game::Engine& engine) override
	{
		return true;
	}

	void before_draw(mhe::game::Engine& engine) override
	{
		mhe::ComputeCallExplicit compute_call;
		mhe::prepare_draw_call(compute_call);
		compute_call.shader_program = shader_program_;
		compute_call.images[0] = texture_;
		compute_call.images[1] = blurred_texture_;
		compute_call.image_access[0] = mhe::access_readonly;
		compute_call.image_access[1] = mhe::access_writeonly;
		compute_call.workgroups_number.set(1024 / 16, 1024 / 16, 1);

		engine.context().driver.execute(engine.context(), &compute_call, 1);
	}
private:
	mhe::Texture* texture_;
	mhe::Texture* blurred_texture_;
	mhe::ShaderProgram* shader_program_;
};

int main(int /*argc*/, char** /*argv*/)
{
	mhe::app::Application app("13_compute");
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
	config.render_config_filename = mhe::utils::path_join(config.assets_path, "render_without_postprocess.xml");
	app.init(config);

	mhe::game::GameSceneDesc desc;
	GameScene* game_scene = new GameScene;
	game_scene->init(app.engine(), desc);
	app.engine().set_game_scene(game_scene);

	return app.run();
}
