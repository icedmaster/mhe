#include "mhe.hpp"

class GameScene : public mhe::game::GameScene
{
public:
	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/)
	{
		mhe::Node& node = engine.scene().create_node();
		mhe::utils::create_plane(node.mesh, engine.context());
		node.transform = engine.scene().transform_pool().create();
		mhe::UnlitMaterialSystem* material_system = engine.context().material_systems.get<mhe::UnlitMaterialSystem>();

		mhe::ModelContext model_context;
		model_context.model = engine.scene().transform_pool().get(node.transform);
		material_system->setup(engine.context(), &node, &model_context, 1);
		return true;
	}

    bool update(mhe::game::Engine& /*engine*/)
	{
		return true;
	}
};

void setup_material_systems(mhe::Context& context)
{
	mhe::MaterialSystemContext material_system_context;
	material_system_context.shader_name = "unlit";
	mhe::UnlitMaterialSystem* material_system = new mhe::UnlitMaterialSystem;
	bool result = material_system->init(context, material_system_context);
	ASSERT(result, "Can not initialize unlit material system");
	context.material_systems.add(material_system);
}

int main(int /*argc*/, char** /*argv*/)
{
    mhe::app::Application app("02_draw");
	mhe::app::ApplicationConfig config;
	config.width = 800;
	config.height = 600;
	config.bpp = 32;
	config.fullscreen = false;
    config.assets_path = "../assets/";
	app.init(config);

	setup_material_systems(app.engine().context());

	mhe::game::GameSceneDesc desc;
	GameScene* game_scene = new GameScene;
	game_scene->init(app.engine(), desc);
	app.engine().set_game_scene(game_scene);
	return app.run();
}
