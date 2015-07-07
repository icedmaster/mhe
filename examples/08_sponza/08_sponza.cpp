#include "mhe.hpp"

class GameScene : public mhe::game::GameScene
{
public:
	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/)
	{
		keyboard_ = engine.event_manager().keyboard();
		
		mhe::NodeInstance& node = engine.scene().create_node();
		mhe::load_node<mhe::GBufferFillMaterialSystem>(node, mhe::string("sponza.bin"), engine.context(), engine.scene_context());
		mhe::MaterialId floor_id;
		engine.context().material_manager.id_by_name(floor_id, mhe::string("floor"));
		mhe::MaterialData& material = engine.context().material_manager.material_data(floor_id);
		material.render_data.glossiness = 1.0f;

		init_lighting(engine);

		return true;
	}

	bool update(mhe::game::Engine& engine)
	{
		if (keyboard_->is_key_pressed(mhe::KeyboardDevice::key_0))
			light_type_ = mhe::Light::spot;
		else if (keyboard_->is_key_pressed(mhe::KeyboardDevice::key_2))
			light_type_ = mhe::Light::directional;
		else return true;
		update_lights(engine);
		return true;
	}
private:
	void init_lighting(mhe::game::Engine& engine)
	{
		init_directional_lights(engine);
	}

	void init_spot_lights(mhe::game::Engine& engine)
	{
        mhe::LightInstance& light_instance = engine.scene().create_light();
		mhe::Light& light = light_instance.light;
		light.shading().diffuse = mhe::color_green;
		light.shading().specular = mhe::color_white;
		mhe::set_light_position(engine.scene_context(), light_instance.id, mhe::vec3(0, 20, 0));
		mhe::set_light_rotation(engine.scene_context(), light_instance.id, mhe::quatf(-mhe::pi_2, 0.0f, 0.0f));
		light.desc().spot.attenuation_a = 0.2f;
		light.desc().spot.attenuation_b = 0.1f;
		light.desc().spot.angle = mhe::deg_to_rad(30.0f);
		light.desc().spot.angle_attenuation = 0.5f;
		light.desc().spot.spot_shadowmap_projection_znear = 0.1f;
		light.desc().spot.spot_shadowmap_projection_zfar = 50.0f;
		light.desc().cast_shadows = false;
		light.set_type(mhe::Light::spot);

        mhe::LightInstance& light_instance2 = engine.scene().create_light();
		mhe::Light& light2 = light_instance2.light;
		light2.shading().diffuse = mhe::color_blue;
		light2.shading().specular = mhe::color_white;
		mhe::set_light_position(engine.scene_context(), light_instance2.id, mhe::vec3(0, 1, 20));
		mhe::set_light_rotation(engine.scene_context(), light_instance2.id, mhe::quatf(0.0f, mhe::pi, 0.0f));
		light2.desc().spot.attenuation_a = 0.2f;
		light2.desc().spot.attenuation_b = 0.2f;
		light2.desc().spot.angle = mhe::deg_to_rad(30.0f);
		light2.desc().spot.angle_attenuation = 0.5f;
		light2.desc().spot.spot_shadowmap_projection_znear = 0.1f;
		light2.desc().spot.spot_shadowmap_projection_zfar = 50.0f;
		light2.desc().cast_shadows = true;
		light2.set_type(mhe::Light::spot);

		spot_lights_[0] = light_instance.id;
		spot_lights_[1] = light_instance2.id;
	}

	void init_directional_lights(mhe::game::Engine& engine)
	{
        mhe::LightInstance& light_instance = engine.scene().create_light();
		mhe::Light& light = light_instance.light;
		light.shading().diffuse = mhe::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		light.shading().specular = mhe::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		mhe::set_light_position(engine.scene_context(), light_instance.id, mhe::vec3(0, 2000, 0));
		mhe::set_light_rotation(engine.scene_context(), light_instance.id, mhe::quatf(-mhe::pi_2, 0.0f, 0.0f));
		light.set_type(mhe::Light::directional);
		light.desc().directional.directional_shadowmap_projection_znear = 10.0f;
		light.desc().directional.directional_shadowmap_projection_zfar = 2200.0f;
		light.desc().directional.directional_shadowmap_projection_height = 2500.0;
		light.desc().directional.directional_shadowmap_projection_width = 2500.0;
		light.desc().cast_shadows = true;
		light.desc().shadowmap_bias = 0.002;
		light_instance.enabled = true;
	}

	void update_lights(mhe::game::Engine& engine)
	{
		mhe::LightInstance::IdType for_enable[2];
		mhe::LightInstance::IdType for_disable[2];

		switch (light_type_)
		{
		case mhe::Light::spot:
			std::copy(spot_lights_, spot_lights_ + 2, for_enable);
			std::copy(directional_lights_, directional_lights_ + 2, for_disable);
			break;

		case mhe::Light::directional:
			std::copy(directional_lights_, directional_lights_ + 2, for_enable);
			std::copy(spot_lights_, spot_lights_ + 2, for_disable);
			break;

		default: break;
		}

		for (int i = 0; i < 2; ++i)
			engine.scene_context().light_pool.get(for_enable[i]).enabled = true;

		for (int i = 0; i < 2; ++i)
			engine.scene_context().light_pool.get(for_disable[i]).enabled = false;
	}

	mhe::LightInstance::IdType spot_lights_[2];
	mhe::LightInstance::IdType directional_lights_[2];
	const mhe::KeyboardDevice* keyboard_;
	int light_type_;
};

int main(int /*argc*/, char** /*argv*/)
{
	mhe::app::Application app("08_sponza");
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
	config.render_config_filename = mhe::utils::path_join(config.assets_path, "render.xml");
	app.init(config);

	app.engine().renderer()->set_ambient_color(mhe::color_white * 0.5f);

	mhe::game::GameSceneDesc desc;
	GameScene* game_scene = new GameScene;
	game_scene->init(app.engine(), desc);
	app.engine().set_game_scene(game_scene);

	mhe::PerspectiveCameraParameters camera_parameters;
	camera_parameters.fov = 60.0f;
	camera_parameters.znear = 1.0f;
	camera_parameters.zfar = 3000.0f;
	mhe::game::FPSCameraController* camera_controller = new mhe::game::FPSCameraController(app.engine(), camera_parameters,
		mhe::vec3(0, 10, 10), mhe::vec3(mhe::deg_to_rad(30.0f), mhe::pi_2, 0));
	camera_controller->set_move_speed(500.0f);
	app.engine().scene().set_camera_controller(camera_controller);
	return app.run();
}
