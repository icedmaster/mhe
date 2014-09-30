#include "mhe.hpp"

class GameScene : public mhe::game::GameScene
{
public:
	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/)
	{
		mhe::Node& node = engine.scene().create_node();
		mhe::utils::create_plane(node.mesh, engine.context());

		mhe::Node& node2 = engine.scene().create_node();
		mhe::utils::create_plane(node2.mesh, engine.context());
		mhe::Transform& transform = engine.scene().transform_pool().get(node2.transform);
		transform.translate_by(mhe::vec3(2, 2, 0));

		mhe::Node& node3 = engine.scene().create_node();
		engine.context().mesh_manager.get(node3.mesh, "duck.bin");
		mhe::Transform& transform3 = engine.scene().transform_pool().get(node3.transform);
		transform3.translate_by(mhe::vec3(-2, -2, 0));
		transform3.scale_to(mhe::vec3(0.05, 0.05, 0.05));

		mhe::Node& node4 = engine.scene().create_node();
		mhe::utils::create_axes(node4.mesh, engine.context());

		mhe::UnlitMaterialSystem* material_system = engine.context().material_systems.get<mhe::UnlitMaterialSystem>();

		mhe::ModelContext model_context[4];
		model_context[0].model = engine.scene().transform_pool().get(node.transform);
		model_context[0].textures[0] = "test.tga";
		model_context[1].model = transform;
		model_context[2].model = transform3;
		model_context[3].model = engine.scene().transform_pool().get(node4.transform);
		material_system->setup(engine.context(), &node, model_context, 4);
		return true;
	}

	bool update(mhe::game::Engine& engine)
	{
		return true;
	}
};

int main(int argc, char** argv)
{
	mhe::app::Application app("02_draw");
	mhe::app::ApplicationConfig config;
	config.width = 800;
	config.height = 600;
	config.bpp = 32;
	config.fullscreen = false;
	config.assets_path = "e:/projects/mhe/assets/";
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
