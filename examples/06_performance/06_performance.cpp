#include "mhe.hpp"

class GameScene : public mhe::game::GameScene
{
	static const size_t nodes_number = 100;
	static const size_t lights_number = 64;
public:
	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/)
	{
		keyboard_ = engine.event_manager().keyboard();

		mhe::Node& skybox = engine.scene().create_node();
		mhe::utils::create_skybox_quad(skybox.mesh, engine.context());

		mhe::SkyboxMaterialSystem* skybox_material_system = engine.context().material_systems.get<mhe::SkyboxMaterialSystem>();
		mhe::ModelContext skybox_context;
		skybox_context.textures[0] = "cubemaps/test.cubemap";
		skybox_material_system->setup(engine.context(), &skybox, &skybox_context, 1);
		
		mhe::ModelContext model_context[nodes_number];
		mhe::Node* first = nullptr;
		mhe::vec3 pos(-100, 0, 0);
		for (size_t i = 0; i < nodes_number; ++i)
		{
			mhe::Node& node = engine.scene().create_node();
			if (i == 0) first = &node;
			engine.context().mesh_manager.get(node.mesh, "sphere.bin");
			mhe::Transform& transform = engine.scene().transform_pool().get(node.transform);
			pos += mhe::vec3(2, 0, 0);
			transform.translate_to(pos);
			model_context[i].model = transform;
			model_context[i].textures[0] = "test.tga";
		}

		mhe::GBufferFillMaterialSystem* material_system = engine.context().material_systems.get<mhe::GBufferFillMaterialSystem>();

		material_system->setup(engine.context(), first, model_context, nodes_number);

		mhe::Node& quad = engine.scene().create_node();
		mhe::utils::create_fullscreen_quad(quad.mesh, engine.context());
		mhe::GBufferDrawMaterialSystem* draw_material_system = engine.context().material_systems.get<mhe::GBufferDrawMaterialSystem>();
		draw_material_system->setup(engine.context(), &quad, model_context, 1);

		mhe::Node& ambient_quad = engine.scene().create_node();
		mhe::utils::create_fullscreen_quad(ambient_quad.mesh, engine.context());
		mhe::PosteffectSimpleMaterialSystem* posteffect_material_system = engine.context().material_systems.get<mhe::PosteffectSimpleMaterialSystem>();
		posteffect_material_system->setup(engine.context(), &ambient_quad, model_context, 1);

		init_lighting(engine);
		return true;
	}

	bool update(mhe::game::Engine& engine)
	{
		if (keyboard_->is_key_pressed(mhe::KeyboardDevice::key_0))
			light_type_ = mhe::Light::spot;
		else if (keyboard_->is_key_pressed(mhe::KeyboardDevice::key_1))
			light_type_ = mhe::Light::omni;
		else if (keyboard_->is_key_pressed(mhe::KeyboardDevice::key_2))
			light_type_ = mhe::Light::directional;
		else return true;
		return true;
	}
private:
	void init_lighting(mhe::game::Engine& engine)
	{
		for (size_t i = 0; i < lights_number; ++i)
		{
			mhe::Light& light = create_and_get(engine.context().light_pool);
			light.shading().diffuse = mhe::color_green;
			light.shading().specular = mhe::color_white;
			float xpos = -static_cast<float>(lights_number * 0.5f) + i;
			light.set_position(mhe::vec3(xpos, 0, 3));
			light.desc().omni.radius = 3.0f;
			light.desc().omni.omni_attenuation = 1.5f;
			light.set_type(mhe::Light::omni);
		}
	}

	const mhe::KeyboardDevice* keyboard_;
	int light_type_;
};

int main(int argc, char** argv)
{
	mhe::app::Application app("06_lighting");
	mhe::app::ApplicationConfig config;
	config.width = 1024;
	config.height = 768;
	config.bpp = 32;
	config.fullscreen = false;
	config.assets_path = "e:/projects/mhe/assets/";
	config.render_config_filename = mhe::utils::path_join(config.assets_path, "render.xml");
	app.init(config);

	mhe::game::GameSceneDesc desc;
	GameScene* game_scene = new GameScene;
	game_scene->init(app.engine(), desc);
	app.engine().set_game_scene(game_scene);

	mhe::PerspectiveCameraParameters camera_parameters;
	camera_parameters.fov = 60.0f;
	camera_parameters.znear = 0.1f;
	camera_parameters.zfar = 100.0f;
	app.engine().scene().set_camera_controller(new mhe::game::FPSCameraController(app.engine(), camera_parameters,
		mhe::vec3(0, 1, 10), mhe::vec3(0, 1, 0), mhe::vec3::up()));
	return app.run();
}
