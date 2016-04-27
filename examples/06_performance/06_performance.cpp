#include "mhe.hpp"

class GameScene : public mhe::game::GameScene
{
	static const size_t nodes_number = 100;
    static const size_t lights_number = 64;
public:
	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/)
	{	
        mhe::GBufferFillMaterialSystem* material_system = engine.context().material_systems.get<mhe::GBufferFillMaterialSystem>();

        const mhe::string sphere_mesh_name = "sphere.bin";
		mhe::vec3 pos(-100, 0, 0);
		for (size_t i = 0; i < nodes_number; ++i)
		{
			mhe::NodeInstance& node = engine.scene().create_node();
            engine.context().mesh_manager.get_instance(node.mesh, sphere_mesh_name);
			mhe::Transform& transform = engine.scene().transform_pool().get(node.transform_id).transform;
			pos += mhe::vec3(2, 0, 0);
			transform.translate_to(pos);
            mhe::AABBInstance& aabb = engine.scene_context().aabb_pool.get(node.aabb_id);
            aabb.aabb.center = pos;
            aabb.aabb.extents = mhe::vec3(0.5f, 0.5f, 0.5f);

            mhe::setup_node(node, material_system, engine.context(), engine.scene_context(), mhe::string("test.tga"));
		}

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
            mhe::LightInstance& light_instance = engine.scene().create_light(mhe::Light::omni);
			mhe::Light& light = light_instance.light;
			light.shading().diffuse = mhe::color_green;
			light.shading().specular = mhe::color_white;
			float xpos = -static_cast<float>(lights_number * 0.5f) + i;
			mhe::set_light_position(engine.scene_context(), light_instance.id, mhe::vec3(xpos, 0, 3));
			light.desc().omni.radius = 3.0f;
			light.desc().omni.omni_attenuation = 1.5f;
			light.set_type(mhe::Light::omni);

            mhe::AABBf& aabb = engine.scene_context().aabb_pool.get(light_instance.aabb_id).aabb;
            aabb.extents = mhe::vec3(0.5f, 0.5f, 0.5f);
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
