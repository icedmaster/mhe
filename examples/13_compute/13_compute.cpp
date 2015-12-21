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

		desc.height /= 4;
		desc.width /= 4;
		mhe::Texture& downsampled_texture = create_and_get(engine.context().texture_pool);
		if (!downsampled_texture.init(desc, nullptr, 0))
			return false;
		downsampled_texture_ = &downsampled_texture;

		mhe::PosteffectDebugMaterialSystem* material_system = engine.context().material_systems.get<mhe::PosteffectDebugMaterialSystem>();
		mhe::TextureInstance visualized_texture_instance;
		visualized_texture_instance.id = downsampled_texture.id();
		material_system->set_texture(0, visualized_texture_instance);
		material_system->enable();

		mhe::Shader shader;
		engine.context().shader_manager.get(shader, mhe::string("13_compute"));
		mhe::UberShader& ubershader = engine.context().ubershader_pool.get(shader.shader_program_handle);
		shader_program_ = &engine.context().shader_pool.get(ubershader.get_default());
		const mhe::UberShader::Info& info = ubershader.info("LUMINANCE");
		mhe::UberShader::Index ubershader_index;
		ubershader_index.set(info, 1);
		luminance_calculation_program_ = &engine.context().shader_pool.get(ubershader.get(ubershader_index));

		engine.context().shader_manager.get(shader, mhe::string("compute/downsample"));
		downsample_program_ = &engine.context().shader_pool.get(engine.context().ubershader_pool.get(shader.shader_program_handle).get_default());

		mhe::ShaderStorageBufferDesc shader_storage_buffer_desc;
		shader_storage_buffer_desc.size = 1024 * 1024 / 16 / 16 * sizeof(float);
		shader_storage_buffer_desc.format = mhe::format_float;
		shader_storage_buffer_desc.update_type = mhe::buffer_update_type_dynamic;
		mhe::ShaderStorageBuffer& buffer = create_and_get(engine.context().shader_storage_buffer_pool);
		if (!buffer.init(shader_storage_buffer_desc, nullptr, 0))
			return false;
		luminance_ = &buffer;
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

		// calculate average luminance of the image
		compute_call.shader_program = luminance_calculation_program_;
		compute_call.images[0] = texture_;
		compute_call.image_access[0] = mhe::access_readonly;
		compute_call.buffers[0] = luminance_;
		compute_call.workgroups_number.set(1024 / 16, 1024 / 16, 1);
		engine.context().driver.execute(engine.context(), &compute_call, 1);

		// get the blurred image
		compute_call.shader_program = shader_program_;
		compute_call.images[1] = blurred_texture_;
		compute_call.image_access[1] = mhe::access_writeonly;
		engine.context().driver.execute(engine.context(), &compute_call, 1);

		// downsample the result
		compute_call.shader_program = downsample_program_;
		compute_call.images[0] = blurred_texture_;
		compute_call.images[1] = downsampled_texture_;
		compute_call.workgroups_number.set(1024 / 4 / 16, 1024 / 4 / 16, 1);

		engine.context().driver.execute(engine.context(), &compute_call, 1);
	}
private:
	mhe::Texture* texture_;
	mhe::Texture* blurred_texture_;
	mhe::Texture* downsampled_texture_;
	mhe::ShaderProgram* shader_program_;
	mhe::ShaderProgram* downsample_program_;
	mhe::ShaderProgram* luminance_calculation_program_;
	mhe::ShaderStorageBuffer* luminance_;
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
