#include <mhe.hpp>
#include <render/mesh_grid.hpp>
#include "cpuraytracer.hpp"

//#define DONT_RAYTRACE

class RaytracingScene : public mhe::game::GameScene
{
public:
	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& desc) override
	{
		mhe::NodeInstance& node = engine.scene().create_node();
#ifdef DONT_RAYTRACE
		mhe::load_node<mhe::GBufferFillMaterialSystem>(node, mhe::string("lighting-test-scene.bin"), engine.context(), engine.scene_context());
		node.cast_shadow = false;
		node.receive_shadow = false;
#endif
		mhe::utils::create_sphere(node, engine.context(), 3, mhe::utils::mesh_creation_flag_trace_data);

		mhe::PerspectiveCameraParameters camera_params;
		camera_params.fov = 45.0f;
		camera_params.znear = 0.1f;
		camera_params.zfar = 100.0f;
		camera_.init(engine.context(), camera_params, mhe::vec3(0.0f, 0.0f, 10.0f), mhe::vec3(0.0f, mhe::pi, 0.0f));

		engine.context().driver.set_clear_color(mhe::color_green);
		engine.context().material_systems.disable_all();

		raytracer_.init(&engine);

		return true;
	}

	bool update(mhe::game::Engine& engine) override
	{
		return true;
	}

	void draw(const mhe::game::Engine& engine) override
	{
		raytracer_.render(camera_);
	}
private:
	mhe::NodeInstance* sphere_;
	mhe::Camera camera_;
	CPURaytracer raytracer_;
};

void test_intersection()
{
	mhe::trianglef tri;
	tri.vertices[2] = mhe::vec3(-0.5f, 0.0f, 0.0f);
	tri.vertices[1] = mhe::vec3(0.0f, 1.0f, 0.0f);
	tri.vertices[0] = mhe::vec3(0.5f, 0.0f, 0.0f);

	mhe::rayf r(mhe::vec3(0.0f, 0.0f, 5.0f), mhe::vec3(0.0f, 0.1f, -1.0f));
	float dist;
	mhe::vec3 p;
	mhe::intersects(p, dist, r, tri);
}

int main(int, char**)
{
	mhe::app::Application app("raytracing");

	//test_intersection();

	mhe::app::ApplicationConfig config;
	config.width = 160;
	config.height = 160;
	config.bpp = 32;
	config.fullscreen = false;
#ifndef MHE_VS
    config.assets_path = "../assets/";
#else
	config.assets_path = "../../assets/";
#endif
	config.render_config_filename = mhe::utils::path_join(config.assets_path, "render_raytracer.xml");
	app.init(config);
	RaytracingScene* scene = new RaytracingScene;
	mhe::game::GameSceneDesc desc;
	scene->init(app.engine(), desc);
	app.engine().set_game_scene(scene);

#ifdef DONT_RAYTRACE
	mhe::PerspectiveCameraParameters camera_parameters;
	camera_parameters.fov = 60.0f;
	camera_parameters.znear = 1.0f;
	camera_parameters.zfar = 3000.0f;
	mhe::game::FPSCameraController* camera_controller = new mhe::game::FPSCameraController(app.engine(), camera_parameters,
		mhe::vec3(0, 10, 20), mhe::vec3(0.0f, mhe::pi, 0));
	camera_controller->set_move_speed(500.0f);
	app.engine().scene().set_camera_controller(camera_controller);
#endif

	return app.run();
}
