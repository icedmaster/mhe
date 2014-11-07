#include "mhe.hpp"

class GameScene : public mhe::game::GameScene
{
public:
	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/)
	{
		mhe::NodeInstance& skybox = engine.scene().create_node();
		mhe::utils::create_skybox_quad(skybox.node.mesh, engine.context());

		mhe::SkyboxMaterialSystem* skybox_material_system = engine.context().material_systems.get<mhe::SkyboxMaterialSystem>();
		mhe::ModelContext skybox_context;
		skybox_context.textures[0] = "cubemaps/test.cubemap";
		skybox_material_system->setup(engine.context(), engine.scene().scene_context(), &skybox, &skybox_context, 1);
		
		mhe::NodeInstance& node = engine.scene().create_node();
		engine.context().mesh_manager.get(node.node.mesh, "sphere.bin");
		mhe::Transform& transform = engine.scene().transform_pool().get(node.transform_id).transform;
		transform.scale_to(mhe::vec3(0.5, 0.5, 0.5));

		mhe::GBufferFillMaterialSystem* material_system = engine.context().material_systems.get<mhe::GBufferFillMaterialSystem>();

		mhe::ModelContext model_context[1];
		model_context[0].textures[0] = "test.tga";
		material_system->setup(engine.context(), engine.scene().scene_context(), &node, model_context, 1);

		mhe::NodeInstance& quad = engine.scene().create_node();
		mhe::utils::create_fullscreen_quad(quad.node.mesh, engine.context());
		mhe::PosteffectSimpleMaterialSystem* draw_material_system = engine.context().material_systems.get<mhe::PosteffectSimpleMaterialSystem>();
		draw_material_system->setup(engine.context(), engine.scene().scene_context(), &quad, model_context, 1);
		return true;
	}

	bool update(mhe::game::Engine& engine)
	{
		return true;
	}
};

int main(int argc, char** argv)
{
	mhe::app::Application app("03_skybox");
	mhe::app::ApplicationConfig config;
	config.width = 1024;
	config.height = 768;
	config.bpp = 32;
	config.fullscreen = false;
	config.assets_path = "e:/projects/mhe/assets/";
	config.render_config_filename = mhe::utils::path_join(config.assets_path, "render_basic.xml");
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
