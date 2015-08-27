#include "mhe.hpp"

class GameScene : public mhe::game::GameScene
{
public:
	GameScene() : allocator_(200 * sizeof(mhe::mat4x4)) {}

	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/) override
	{
		mhe::NodeInstance& node = engine.scene().create_node();
		mhe::load_node<mhe::GBufferFillMaterialSystem>(node, mhe::string("rainbow_dash.bin"), engine.context(), engine.scene_context());
		engine.scene_context().transform_pool.get(node.transform_id).transform.rotate_by(mhe::quatf(0.0f, mhe::pi, 0.0f));
		engine.scene_context().transform_pool.get(node.transform_id).transform.scale_by(mhe::vec3(1.0f, 1.0f, 1.0f));

		engine.scene_context().aabb_pool.get(node.aabb_id).aabb.extents *= 1.0f;

		node_ = node.id;

		mhe::TextureInstance texture;
		engine.context().texture_manager.get(texture, mhe::string("rainbow-dash/rainbow_dash.tga"));
		mhe::Material& material = engine.context().materials[node.mesh.instance_parts[0].material.material_system].get(
			node.mesh.instance_parts[0].material.id);
		material.textures[mhe::albedo_texture_unit] = texture;

		mhe::LightInstance& light_instance = engine.scene().create_light();
		mhe::Light& light = light_instance.light;
		light.shading().diffuse = mhe::vec4(240.0f / 255.0f, 150.0f / 255.0f, 80.0f / 255.0f, 1.0f);
		light.shading().specular = mhe::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		light.shading().intensity = 5.0f;
		mhe::set_light_position(engine.scene_context(), light_instance.id, mhe::vec3(0, 10.0f, 0));
		mhe::set_light_rotation(engine.scene_context(), light_instance.id, mhe::quatf(-mhe::pi_2, 0.0f, 0.0f));
		light.set_type(mhe::Light::directional);
		light.desc().cast_shadows = true;
		light.desc().shadowmap_bias = 0.005f;
		light_instance.enabled = true;
		return true;
	}

	bool update(mhe::game::Engine& engine) override
	{
		allocator_.reset();
		mhe::NodeInstance& node = engine.scene_context().node_pool.get(node_);
		mhe::Skeleton& skeleton = node.mesh.mesh.skeleton;
		mhe::TextureBuffer& texture = engine.context().texture_buffer_pool.get(node.mesh.skeleton_instance.texture_buffer);
		mhe::mat4x4* matrices = static_cast<mhe::mat4x4*>(allocator_.alloc(sizeof(mhe::mat4x4) * skeleton.bones.size()));
		update_skeleton(matrices, skeleton);
		uint8_t* data = reinterpret_cast<uint8_t*>(matrices);
		texture.update(data);
		return true;
	}
private:
	void update_skeleton(mhe::mat4x4* matrices, const mhe::Skeleton& skeleton)
	{
		size_t size = skeleton.bones.size();
		for (size_t i = 0; i < size; ++i)
		{
			matrices[i] = mhe::mat4x4::identity();
			update_bone(matrices[i], i, skeleton);
			matrices[i] *= skeleton.bones[i].inv_transform;
			matrices[i].transpose();
		}
	}

	void update_bone(mhe::mat4x4& m, size_t index, const mhe::Skeleton& skeleton)
	{
		m *= skeleton.bones[index].local_transform;
		if (skeleton.bones[index].parent_id != mhe::Bone::invalid_id)
			update_bone(m, skeleton.bones[index].parent_id, skeleton);
	}

	mhe::NodeInstance::IdType node_;
	mhe::fixed_size_allocator<true> allocator_;
};

int main(int /*argc*/, char** /*argv*/)
{
	mhe::app::Application app("11_animation");
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
	config.render_config_filename = mhe::utils::path_join(config.assets_path, "render_without_postprocess.xml");
	app.init(config);

	mhe::game::GameSceneDesc desc;
	GameScene* game_scene = new GameScene;
	game_scene->init(app.engine(), desc);
	app.engine().set_game_scene(game_scene);

	mhe::PerspectiveCameraParameters camera_parameters;
	camera_parameters.fov = 60.0f;
	camera_parameters.znear = 0.5f;
	camera_parameters.zfar = 100.0f;
	mhe::game::FPSCameraController* camera_controller = new mhe::game::FPSCameraController(app.engine(), camera_parameters,
		mhe::vec3(0, 0, 3), mhe::vec3(0.0f, mhe::pi, 0));
	camera_controller->set_move_speed(10.0f);
	app.engine().scene().set_camera_controller(camera_controller);

	return app.run();
}
