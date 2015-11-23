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
		mhe::utils::create_sphere(node, engine.context(), 3, mhe::utils::mesh_creation_flag_trace_data);
		mhe::NodeInstance& node2 = engine.scene().create_node();
		engine.context().mesh_manager.get_instance(node2.mesh, mhe::string("sphere.bin"));
		node2.mesh.mesh.aabb.extents = mhe::vec3(1, 1, 1);
		mhe::NodeInstance& floor = engine.scene().create_node();
		mhe::utils::create_plane(floor, engine.context(), mhe::utils::mesh_creation_flag_trace_data);

		mhe::MaterialInitializationData material_initialization_data;
		material_initialization_data.render_data.glossiness = 0.05f;
		mhe::MaterialId material_id = engine.context().material_manager.get(material_initialization_data);
		node.mesh.mesh.parts[0].material_id = material_id;
		node2.mesh.mesh.parts[0].material_id = material_id;
		floor.mesh.mesh.parts[0].material_id = material_id;

		mhe::Transform& transform = engine.scene_context().transform_pool.get(node2.transform_id).transform;
		transform.translate_to(mhe::vec3(-2.0f, 3.0f, -2.0f));

		mhe::Transform& floor_transform = engine.scene_context().transform_pool.get(floor.transform_id).transform;
		floor_transform.translate_to(mhe::vec3(0.0f, -2.0f, 0.0f));
		floor_transform.rotate_by(mhe::quatf(-mhe::pi_2, 0.0f, 0.0f));
		floor_transform.scale_to(mhe::vec3(4.0f, 4.0f, 4.0f));

		mhe::LightInstance& light = engine.scene().create_light();
		light.light.set_type(mhe::Light::directional);
		mhe::set_light_rotation(engine.scene_context(), light.id, mhe::quatf(-mhe::pi_2, 0.0f, 0.0f));
		light.light.shading().diffuse = mhe::color_green;

#ifndef DONT_RAYTRACE
		mhe::PerspectiveCameraParameters camera_params;
		camera_params.fov = 45.0f;
		camera_params.znear = 0.1f;
		camera_params.zfar = 30.0f;
		camera_.init(engine.context(), camera_params, mhe::vec3(0.0f, 1.0f, 10.0f), mhe::vec3(0.0f, mhe::pi, 0.0f));

		engine.context().driver.set_clear_color(mhe::color_green);
		engine.context().material_systems.disable_all();

		raytracer_.init(&engine);
#else
		mhe::setup_node(node, engine.context().material_systems.get<mhe::GBufferFillMaterialSystem>(), engine.context(), engine.scene_context(), 
			mhe::string());
#endif

		return true;
	}

	bool update(mhe::game::Engine& engine) override
	{
		return true;
	}

	void draw(const mhe::game::Engine& engine) override
	{
#ifndef DONT_RAYTRACE
		raytracer_.render(camera_);
#endif
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
	mhe::hitf h;
	mhe::intersects(h, r, tri);
}

int main(int, char**)
{
	mhe::app::Application app("raytracing");

	test_intersection();

	mhe::app::ApplicationConfig config;
#ifndef DONT_RAYTRACE
	config.width = 160;
	config.height = 160;
#else
	config.width = 1280;
	config.height = 720;
#endif
	config.bpp = 32;
	config.fullscreen = false;
#ifndef MHE_VS
    config.assets_path = "../assets/";
#else
	config.assets_path = "../../assets/";
#endif

#ifndef DONT_RAYTRACE
	config.render_config_filename = mhe::utils::path_join(config.assets_path, "render_raytracer.xml");
#else
	config.render_config_filename = mhe::utils::path_join(config.assets_path, "render.xml");
#endif
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
		mhe::vec3(0, 0, 20), mhe::vec3(0.0f, mhe::pi, 0));
	camera_controller->set_move_speed(500.0f);
	app.engine().scene().set_camera_controller(camera_controller);
#endif

	return app.run();
}
