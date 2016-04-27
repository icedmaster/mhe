#include "mhe.hpp"

class GameScene : public mhe::game::GameScene
{
public:
	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/)
	{
		keyboard_ = engine.event_manager().keyboard();
		
		mhe::NodeInstance& node = engine.scene().create_node();
        engine.context().mesh_manager.get_instance(node.mesh, mhe::string("sphere.bin"));
		mhe::Transform& transform = engine.scene().transform_pool().get(node.transform_id).transform;
		transform.scale_to(mhe::vec3(2, 2, 2));

		mhe::NodeInstance& plane = engine.scene().create_node();
        mhe::utils::create_plane(plane, engine.context());
		mhe::Transform& plane_transform = engine.scene().transform_pool().get(plane.transform_id).transform;
		plane_transform.scale_to(mhe::vec3(50, 50, 50));
		plane_transform.translate_by(mhe::vec3(0, 0, -15));

		mhe::GBufferFillMaterialSystem* material_system = engine.context().material_systems.get<mhe::GBufferFillMaterialSystem>();
        mhe::setup_node(node, material_system, engine.context(), engine.scene_context(), mhe::string("test.tga"));
        mhe::setup_node(plane, material_system, engine.context(), engine.scene_context(), mhe::string("test.tga"));

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
		update_lights(engine);
		return true;
	}
private:
	void init_lighting(mhe::game::Engine& engine)
	{
		init_spot_lights(engine);
		init_omni_lights(engine);
		init_directional_lights(engine);

		light_type_ = mhe::Light::spot;
		update_lights(engine);
	}

	void init_spot_lights(mhe::game::Engine& engine)
	{
        mhe::LightInstance& light_instance = engine.scene().create_light(mhe::Light::spot);
		mhe::Light& light = light_instance.light;
		light.shading().diffuse = mhe::color_green;
		light.shading().specular = mhe::color_white;
		mhe::set_light_position(engine.scene_context(), light_instance.id, mhe::vec3(0, 20, 0));
		mhe::set_light_rotation(engine.scene_context(), light_instance.id, mhe::quatf(-mhe::pi_2, 0.0f, 0.0f));
		light.desc().spot.attenuation_a = 0.01f;
		light.desc().spot.attenuation_b = 0.02f;
		light.desc().spot.angle = mhe::deg_to_rad(30.0f);
		light.desc().spot.angle_attenuation = 1.5f;

        mhe::LightInstance& light_instance2 = engine.scene().create_light(mhe::Light::spot);
		mhe::Light& light2 = light_instance2.light;
		light2.shading().diffuse = mhe::color_blue;
		light2.shading().specular = mhe::color_white;
		mhe::set_light_position(engine.scene_context(), light_instance2.id, mhe::vec3(0, 0, 20));
		mhe::set_light_rotation(engine.scene_context(), light_instance2.id, mhe::quatf(0.0f, mhe::pi, 0.0f));
		light2.desc().spot.attenuation_a = 0.01f;
		light2.desc().spot.attenuation_b = 0.02f;
		light2.desc().spot.angle = mhe::deg_to_rad(30.0f);
		light2.desc().spot.angle_attenuation = 1.5f;

		spot_lights_[0] = light_instance.id;
		spot_lights_[1] = light_instance2.id;
	}

	void init_omni_lights(mhe::game::Engine& engine)
	{
        mhe::LightInstance& light_instance = engine.scene().create_light(mhe::Light::omni);
		mhe::Light& light = light_instance.light;
		light.shading().diffuse = mhe::color_red;
		light.shading().specular = mhe::color_white;
		mhe::set_light_position(engine.scene_context(), light_instance.id, mhe::vec3(0, 3, 0));
        light.desc().omni.radius = 1.5f;
		light.desc().omni.omni_attenuation = 5.0f;
		light.set_type(mhe::Light::omni);

        mhe::LightInstance& light_instance2 = engine.scene().create_light(mhe::Light::omni);
		mhe::Light& light2 = light_instance2.light;
		light2.shading().diffuse = mhe::color_yellow;
		light2.shading().specular = mhe::color_white;
		mhe::set_light_position(engine.scene_context(), light_instance2.id, mhe::vec3(0, 0, 3));
        light2.desc().omni.radius = 1.5f;
		light2.desc().omni.omni_attenuation = 5.0f;
		light2.set_type(mhe::Light::omni);

		omni_lights_[0] = light_instance.id;
		omni_lights_[1] = light_instance2.id;
	}

	void init_directional_lights(mhe::game::Engine& engine)
	{
        mhe::LightInstance& light_instance = engine.scene().create_light(mhe::Light::directional);
		mhe::Light& light = light_instance.light;
		light.shading().diffuse = mhe::vec4(0.1f, 0.1f, 0.1f, 1.0f);
		light.shading().specular = mhe::vec4(0.2f, 0.2f, 0.2f, 1.0f);
		mhe::set_light_position(engine.scene_context(), light_instance.id, mhe::vec3(0, 100, 0));
		mhe::set_light_rotation(engine.scene_context(), light_instance.id, mhe::quatf(-mhe::pi_2, 0.0f, 0.0f));
		light.set_type(mhe::Light::directional);

        mhe::LightInstance& light_instance2 = engine.scene().create_light(mhe::Light::directional);
		mhe::Light& light2 = light_instance2.light;
		light2.shading().diffuse = mhe::vec4(0.1f, 0.0f, 0.1f, 1.0f);
		light2.shading().specular = mhe::vec4(0.2f, 0.2f, 0.2f, 1.0f);
		mhe::set_light_position(engine.scene_context(), light_instance2.id, mhe::vec3(0, 0, 100));
		mhe::set_light_rotation(engine.scene_context(), light_instance2.id, mhe::quatf(0.0f, mhe::pi, 0.0f));
		light2.set_type(mhe::Light::directional);

		directional_lights_[0] = light_instance.id;
		directional_lights_[1] = light_instance2.id;
	}

	void update_lights(mhe::game::Engine& engine)
	{
		mhe::LightInstance::IdType for_enable[2];
		mhe::LightInstance::IdType for_disable[4];

		switch (light_type_)
		{
		case mhe::Light::spot:
			std::copy(spot_lights_, spot_lights_ + 2, for_enable);
			std::copy(omni_lights_, omni_lights_ + 2, for_disable);
			std::copy(directional_lights_, directional_lights_ + 2, for_disable + 2);
			break;

		case mhe::Light::omni:
			std::copy(omni_lights_, omni_lights_ + 2, for_enable);
			std::copy(spot_lights_, spot_lights_ + 2, for_disable);
			std::copy(directional_lights_, directional_lights_ + 2, for_disable + 2);
			break;

		case mhe::Light::directional:
			std::copy(directional_lights_, directional_lights_ + 2, for_enable);
			std::copy(spot_lights_, spot_lights_ + 2, for_disable);
			std::copy(omni_lights_, omni_lights_ + 2, for_disable + 2);
			break;

		default: break;
		}

		for (int i = 0; i < 2; ++i)
			engine.scene_context().light_pool.get(for_enable[i]).enabled = true;

		for (int i = 0; i < 4; ++i)
			engine.scene_context().light_pool.get(for_disable[i]).enabled = false;
	}

	mhe::LightInstance::IdType spot_lights_[2];
	mhe::LightInstance::IdType omni_lights_[2];
	mhe::LightInstance::IdType directional_lights_[2];
	const mhe::KeyboardDevice* keyboard_;
	int light_type_;
};

int main(int /*argc*/, char** /*argv*/)
{
	mhe::app::Application app("05_lighting");
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
        mhe::vec3(0, 1, 10), mhe::vec3(0, mhe::pi, 0)));
	return app.run();
}
