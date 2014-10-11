#include "mhe.hpp"

class GameScene : public mhe::game::GameScene
{
public:
	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/)
	{
		keyboard_ = engine.event_manager().keyboard();

		mhe::Node& skybox = engine.scene().create_node();
		mhe::utils::create_skybox_quad(skybox.mesh, engine.context());

		mhe::SkyboxMaterialSystem* skybox_material_system = engine.context().material_systems.get<mhe::SkyboxMaterialSystem>();
		mhe::ModelContext skybox_context;
		skybox_context.textures[0] = "cubemaps/test.cubemap";
		skybox_material_system->setup(engine.context(), &skybox, &skybox_context, 1);
		
		mhe::Node& node = engine.scene().create_node();
		engine.context().mesh_manager.get(node.mesh, "sphere.bin");
		mhe::Transform& transform = engine.scene().transform_pool().get(node.transform);
		transform.scale_to(mhe::vec3(2, 2, 2));

		mhe::Node& plane = engine.scene().create_node();
		mhe::utils::create_plane(plane.mesh, engine.context());
		mhe::Transform& plane_transform = engine.scene().transform_pool().get(plane.transform);
		plane_transform.scale_to(mhe::vec3(50, 50, 50));
		plane_transform.translate_by(mhe::vec3(0, 0, -15));

		mhe::GBufferFillMaterialSystem* material_system = engine.context().material_systems.get<mhe::GBufferFillMaterialSystem>();

		mhe::ModelContext model_context[2];
		model_context[0].model = transform;
		model_context[0].textures[0] = "test.tga";
		model_context[1].model = plane_transform;
		model_context[1].textures[0] = "test.tga";
		material_system->setup(engine.context(), &node, model_context, 2);

		mhe::Node& quad = engine.scene().create_node();
		mhe::utils::create_fullscreen_quad(quad.mesh, engine.context());
		mhe::GBufferDrawMaterialSystem* draw_material_system = engine.context().material_systems.get<mhe::GBufferDrawMaterialSystem>();
		draw_material_system->setup(engine.context(), &quad, model_context, 1);

		mhe::Node& ambient_quad = engine.scene().create_node();
		mhe::utils::create_fullscreen_quad(ambient_quad.mesh, engine.context());
		mhe::PosteffectSimpleMaterialSystem* posteffect_material_system = engine.context().material_systems.get<mhe::PosteffectSimpleMaterialSystem>();
		posteffect_material_system->setup(engine.context(), &ambient_quad, model_context, 1);

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
		mhe::Light& light = create_and_get(engine.context().light_pool);
		light.shading().diffuse = mhe::color_green;
		light.shading().specular = mhe::color_white;
		light.set_position(mhe::vec3(0, 20, 0));
		light.set_direction(-mhe::vec3::up());
		light.desc().spot.attenuation = 0.2f;
		light.desc().spot.angle = mhe::deg_to_rad(30.0f);
		light.desc().spot.angle_attenuation = 0.5f;

		mhe::Light& light2 = create_and_get(engine.context().light_pool);
		light2.shading().diffuse = mhe::color_blue;
		light2.shading().specular = mhe::color_white;
		light2.set_position(mhe::vec3(0, 0, 20));
		light2.set_direction(-mhe::vec3::forward());
		light2.desc().spot.attenuation = 0.2f;
		light2.desc().spot.angle = mhe::deg_to_rad(30.0f);
		light2.desc().spot.angle_attenuation = 0.5f;

		spot_lights_[0] = light.id();
		spot_lights_[1] = light2.id();
	}

	void init_omni_lights(mhe::game::Engine& engine)
	{
		mhe::Light& light = create_and_get(engine.context().light_pool);
		light.shading().diffuse = mhe::color_red;
		light.shading().specular = mhe::color_white;
		light.set_position(mhe::vec3(0, 3, 0));
		light.desc().omni.radius = 3.0f;
		light.desc().omni.omni_attenuation = 0.9f;
		light.set_type(mhe::Light::omni);

		mhe::Light& light2 = create_and_get(engine.context().light_pool);
		light2.shading().diffuse = mhe::color_yellow;
		light2.shading().specular = mhe::color_white;
		light2.set_position(mhe::vec3(0, 0, 3));
		light2.desc().omni.radius = 3.0f;
		light2.desc().omni.omni_attenuation = 0.9f;
		light2.set_type(mhe::Light::omni);

		omni_lights_[0] = light.id();
		omni_lights_[1] = light2.id();
	}

	void init_directional_lights(mhe::game::Engine& engine)
	{
		mhe::Light& light = create_and_get(engine.context().light_pool);
		light.shading().diffuse = mhe::vec4(0.1f, 0.1f, 0.1f, 1.0f);
		light.shading().specular = mhe::vec4(0.2f, 0.2f, 0.2f, 1.0f);;
		light.set_position(mhe::vec3(0, 100, 0));
		light.set_direction(-mhe::vec3::up());
		light.set_type(mhe::Light::directional);

		mhe::Light& light2 = create_and_get(engine.context().light_pool);
		light2.shading().diffuse = mhe::vec4(0.1f, 0.0f, 0.1f, 1.0f);
		light2.shading().specular = mhe::vec4(0.2f, 0.2f, 0.2f, 1.0f);
		light2.set_position(mhe::vec3(0, 0, 100));
		light2.set_direction(-mhe::vec3::forward());
		light2.set_type(mhe::Light::directional);

		directional_lights_[0] = light.id();
		directional_lights_[1] = light2.id();
	}

	void update_lights(mhe::game::Engine& engine)
	{
		mhe::Light::IdType for_enable[2];
		mhe::Light::IdType for_disable[4];

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
			engine.context().light_pool.get(for_enable[i]).enable();

		for (int i = 0; i < 4; ++i)
			engine.context().light_pool.get(for_disable[i]).disable();
	}

	mhe::Light::IdType spot_lights_[2];
	mhe::Light::IdType omni_lights_[2];
	mhe::Light::IdType directional_lights_[2];
	const mhe::KeyboardDevice* keyboard_;
	int light_type_;
};

int main(int argc, char** argv)
{
	mhe::app::Application app("05_lighting");
	mhe::app::ApplicationConfig config;
	config.width = 1024;
	config.height = 768;
	config.bpp = 32;
	config.fullscreen = false;
	config.assets_path = "e:/projects/mhe/assets/";
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
