#include "mhe.hpp"

class GameScene : public mhe::game::GameScene
{
public:
	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/)
	{
		mhe::NodeInstance& node = engine.scene().create_node();
		mhe::load_node<mhe::GBufferFillMaterialSystem>(node, mhe::string("lighting-test.bin"), engine.context(), engine.scene_context());

		mhe::Transform& transform = engine.scene_context().transform_pool.get(node.transform_id).transform;
		transform.scale_to(mhe::vec3(2, 2, 2));
		transform.rotate_by(mhe::quatf(0.0f, -mhe::pi_2, 0.0f));

		mhe::AABBInstance& aabb = engine.scene_context().aabb_pool.get(node.aabb_id);
		aabb.aabb.center *= 2;
		aabb.aabb.extents *= 2;

		init_lighting(engine);

		return true;
	}

	bool update(mhe::game::Engine& /*engine*/) override
	{
		return true;
	}
private:
	void init_lighting(mhe::game::Engine& engine)
	{
		init_directional_lights(engine);
	}

	void init_directional_lights(mhe::game::Engine& engine)
	{
		mhe::LightInstance& light_instance = engine.scene().create_light();
		mhe::Light& light = light_instance.light;
		light.shading().diffuse = mhe::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		mhe::set_light_position(engine.scene_context(), light_instance.id, mhe::vec3(0, 20, 20));
		mhe::set_light_rotation(engine.scene_context(), light_instance.id, mhe::quatf(-mhe::pi_2 * 1.7f, 0.0f, 0.0f));
		light.set_type(mhe::Light::directional);
		light.desc().cast_shadows = true;
		light.desc().shadowmap_bias = 0.000125f;
		light_instance.enabled = true;
	}
};

int main(int /*argc*/, char** /*argv*/)
{
	mhe::app::Application app("shtest");
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
	config.render_config_filename = mhe::utils::path_join(config.assets_path, "shdemo.xml");
	app.init(config);

	mhe::game::GameSceneDesc desc;
	GameScene* game_scene = new GameScene;
	game_scene->init(app.engine(), desc);
	app.engine().set_game_scene(game_scene);

	mhe::PerspectiveCameraParameters camera_parameters;
	camera_parameters.fov = 60.0f;
	camera_parameters.znear = 1.0f;
	camera_parameters.zfar = 100.0f;
	mhe::game::FPSCameraController* camera_controller = new mhe::game::FPSCameraController(app.engine(), camera_parameters,
		mhe::vec3(0, 10, 30), mhe::vec3(0.0f, mhe::pi, 0.0f));
	camera_controller->set_move_speed(500.0f);
	app.engine().scene().set_camera_controller(camera_controller);

	app.engine().renderer()->set_ambient_color(mhe::colorf(0.3f, 0.3f, 0.3f, 1.0f));

	return app.run();
}
