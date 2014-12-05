#include "mhe.hpp"

class GameScene : public mhe::game::GameScene
{
	static const size_t nodes_number = 100;
    static const size_t lights_number = 64;
public:
	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/)
	{
		mhe::NodeInstance& skybox = engine.scene().create_node();
		mhe::utils::create_skybox_quad(skybox.node.mesh, engine.context());

		mhe::SkyboxMaterialSystem* skybox_material_system = engine.context().material_systems.get<mhe::SkyboxMaterialSystem>();
		mhe::ModelContext skybox_context;
		skybox_context.textures[0] = "cubemaps/test.cubemap";
		skybox_material_system->setup(engine.context(), engine.scene_context(), &skybox, &skybox_context, 1);
		
		mhe::ModelContext model_context[nodes_number];
		mhe::NodeInstance* first = nullptr;
		mhe::vec3 pos(-100, 0, 0);
		for (size_t i = 0; i < nodes_number; ++i)
		{
			mhe::NodeInstance& node = engine.scene().create_node();
			if (i == 0) first = &node;
			engine.context().mesh_manager.get(node.node.mesh, "sphere.bin");
			mhe::Transform& transform = engine.scene().transform_pool().get(node.transform_id).transform;
			pos += mhe::vec3(2, 0, 0);
			transform.translate_to(pos);
			model_context[i].textures[0] = "test.tga";
		}

		mhe::GBufferFillMaterialSystem* material_system = engine.context().material_systems.get<mhe::GBufferFillMaterialSystem>();

		material_system->setup(engine.context(), engine.scene_context(), first, model_context, nodes_number);

		mhe::NodeInstance& quad = engine.scene().create_node();
		mhe::utils::create_fullscreen_quad(quad.node.mesh, engine.context());
		mhe::GBufferDrawMaterialSystem* draw_material_system = engine.context().material_systems.get<mhe::GBufferDrawMaterialSystem>();
		draw_material_system->setup(engine.context(), engine.scene_context(), &quad, model_context, 1);

		mhe::NodeInstance& ambient_quad = engine.scene().create_node();
		mhe::utils::create_fullscreen_quad(ambient_quad.node.mesh, engine.context());
		mhe::PosteffectSimpleMaterialSystem* posteffect_material_system = engine.context().material_systems.get<mhe::PosteffectSimpleMaterialSystem>();
		posteffect_material_system->setup(engine.context(), engine.scene_context(), &ambient_quad, model_context, 1);

		init_lighting(engine);
		return true;
	}

    bool update(mhe::game::Engine& /*engine*/)
	{
		return true;
	}
private:
	void init_lighting(mhe::game::Engine& engine)
	{
		for (size_t i = 0; i < lights_number; ++i)
		{
            mhe::LightInstance& light_instance = engine.scene().create_light();
			mhe::Light& light = light_instance.light;
			light.shading().diffuse = mhe::color_green;
			light.shading().specular = mhe::color_white;
			float xpos = -static_cast<float>(lights_number * 0.5f) + i;
			mhe::set_light_position(engine.scene_context(), light_instance.id, mhe::vec3(xpos, 0, 3));
			light.desc().omni.radius = 3.0f;
			light.desc().omni.omni_attenuation = 1.5f;
			light.set_type(mhe::Light::omni);
		}
	}
};

int main(int /*argc*/, char** /*argv*/)
{
	mhe::app::Application app("06_lighting");
	mhe::app::ApplicationConfig config;
	config.width = 1024;
	config.height = 768;
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

	mhe::PerspectiveCameraParameters camera_parameters;
	camera_parameters.fov = 60.0f;
	camera_parameters.znear = 0.1f;
	camera_parameters.zfar = 100.0f;
	app.engine().scene().set_camera_controller(new mhe::game::FPSCameraController(app.engine(), camera_parameters,
		mhe::vec3(0, 1, 10), mhe::vec3(0, 1, 0), mhe::vec3::up()));
	return app.run();
}
