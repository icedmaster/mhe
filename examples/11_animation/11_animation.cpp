#include "mhe.hpp"

struct Frame
{
	float time;
	mhe::vec3 position;
	mhe::quatf rotation;
	mhe::vec3 scale;
};

struct PositionAnimationFrame
{
	float time;
	mhe::vec3 value;
};

struct RotationAnimationFrame
{
	float time;
	mhe::quatf value;
};

struct ScaleAnimationFrame
{
	float time;
	mhe::vec3 value;
};

struct NodeAnimationFrame
{
	size_t node_id;
	std::vector<PositionAnimationFrame> positions;
	std::vector<RotationAnimationFrame> rotations;
	std::vector<ScaleAnimationFrame> scalings;
};

struct AnimationData
{
	float fps;
	float duration;
	std::vector<NodeAnimationFrame> frames;
	int play_mode;
};

class AnimationLoader
{
public:
	static bool load(AnimationData& data, std::istream& stream)
	{
		char header[3];
		stream.read(header, sizeof(header));
		if (strncmp(header, "mhe", 3))
		{
			DEBUG_LOG("AnimationLoader - header is invalid");
			return false;
		}
		char version;
		stream.read(&version, 1);
		if (version != 1)
		{
			DEBUG_LOG("AnimationLoader - version is invalid");
			return false;
		}
		stream.read(reinterpret_cast<char*>(&data.duration), sizeof(float));
		stream.read(reinterpret_cast<char*>(&data.fps), sizeof(float));
		char play_mode;
		stream.read(&play_mode, 1);
		data.play_mode = play_mode;
		uint32_t frames_number;
		stream.read(reinterpret_cast<char*>(&frames_number), sizeof(uint32_t));
		data.frames.resize(frames_number);
		for (size_t i = 0; i < frames_number; ++i)
		{
			NodeAnimationFrame& frame = data.frames[i];
			stream.read(reinterpret_cast<char*>(&frame.node_id), sizeof(uint32_t));
			uint32_t number;
			stream.read(reinterpret_cast<char*>(&number), sizeof(uint32_t));
			frame.positions.resize(number);
			stream.read(reinterpret_cast<char*>(&frame.positions[0]), sizeof(PositionAnimationFrame) * number);
			stream.read(reinterpret_cast<char*>(&number), sizeof(uint32_t));
			frame.rotations.resize(number);
			stream.read(reinterpret_cast<char*>(&frame.rotations[0]), sizeof(RotationAnimationFrame) * number);
			stream.read(reinterpret_cast<char*>(&number), sizeof(uint32_t));
			frame.scalings.resize(number);
			stream.read(reinterpret_cast<char*>(&frame.scalings[0]), sizeof(ScaleAnimationFrame) * number);
		}

		return true;
	}
};

class AnimationController
{
public:
	void set_animation(const AnimationData* data, const mhe::Skeleton& skeleton)
	{
		data_ = data;
		time_ = 0.0f;
		transforms_.resize(skeleton.bones.size());
		for (size_t i = 0, size = transforms_.size(); i < size; ++i)
			transforms_[i] = skeleton.bones[i].local_transform;
		bind_pose_transforms_ = transforms_;
	}

	mhe::vec3 lerp(const mhe::vec3& v1, const mhe::vec3& v2, float t) const
	{
		return mhe::lerp(v1, v2, t);
	}

	mhe::quatf lerp(const mhe::quatf& q1, const mhe::quatf& q2, float t) const
	{
		return mhe::slerp(q1, q2, t);
	}

	template <class T, class V>
	bool sample(T& res, const V& data, float time, float duration)
	{
		size_t size = data.size();
		for (int j = size - 1; j >= 0; --j)
		{
			size_t curr = j;
			size_t next = (j + 1) % size;

			float curr_time = data[curr].time;
			float next_time = next != 0 ? data[next].time : duration + data[0].time;

			float start_time = j == 0 ? 0.0f : data[curr].time;
			float end_time = next == 0 ? duration : data[next].time;

			if (time >= start_time && time <= end_time)
			{
				float t = (time - curr_time) / (next_time - curr_time);
				res = lerp(data[curr].value, data[next].value, t);
				return true;
			}
		}
		return false;
	}

	void update(float dt)
	{
		time_ += dt;
		if (time_ > data_->duration)
			time_ -= data_->duration;
		size_t size = data_->frames.size();
		for (size_t i = 0; i < size; ++i)
		{
			const NodeAnimationFrame& frame = data_->frames[i];

			mhe::vec3 position;
			mhe::quatf rotation;
			mhe::vec3 scale(1, 1, 1);
			bool found = sample(position, frame.positions, time_, data_->duration);
			found |= sample(rotation, frame.rotations, time_, data_->duration);
			found |= sample(scale, frame.scalings, time_, data_->duration);

			if (found)
			{
				mhe::mat4x4 m = mhe::mat4x4::scaling_matrix(scale);
				m *= rotation.to_matrix<mhe::mat4x4>();
				m.set_row(3, position);
				transforms_[frame.node_id] = m;
			}
			else
				transforms_[frame.node_id] = bind_pose_transforms_[frame.node_id];
		}
	}

	const mhe::mat4x4* matrices() const
	{
		return &transforms_[0];
	}
private:
	const AnimationData* data_;
	std::vector<mhe::mat4x4> transforms_;
	std::vector<mhe::mat4x4> bind_pose_transforms_;
	float time_;
};

class GameScene : public mhe::game::GameScene
{
public:
	GameScene() : allocator_(200 * sizeof(mhe::mat4x4)) {}

	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/) override
	{
		mhe::NodeInstance& node = engine.scene().create_node();
		mhe::load_node<mhe::GBufferFillMaterialSystem>(node, mhe::string("rainbow_dash.bin"), engine.context(), engine.scene_context());
		engine.scene_context().transform_pool.get(node.transform_id).transform.rotate_by(mhe::quatf(mhe::pi_2, mhe::pi, 0.0f));
		engine.scene_context().transform_pool.get(node.transform_id).transform.scale_by(mhe::vec3(1.0f, 1.0f, 1.0f));

		engine.scene_context().aabb_pool.get(node.aabb_id).aabb.extents *= 1.0f;

		node_ = node.id;

		load_animation(engine);
		controller_.set_animation(&animation_, node.mesh.mesh.skeleton);

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

		controller_.update(engine.render_context().fdelta);

		mhe::NodeInstance& node = engine.scene_context().node_pool.get(node_);
		mhe::Skeleton& skeleton = node.mesh.mesh.skeleton;
		mhe::TextureBuffer& texture = engine.context().texture_buffer_pool.get(node.mesh.skeleton_instance.texture_buffer);
		mhe::mat4x4* matrices = allocator_.create_array<mhe::mat4x4>(skeleton.bones.size());
		update_skeleton(matrices, skeleton);
		uint8_t* data = reinterpret_cast<uint8_t*>(matrices);
		texture.update(data);
		return true;
	}
private:
	void update_skeleton(mhe::mat4x4* matrices, const mhe::Skeleton& skeleton)
	{
		size_t size = skeleton.bones.size();
		const mhe::mat4x4* transforms = controller_.matrices();
		
		for (size_t i = 0; i < size; ++i)
		{
			update_bone(matrices[i], i, skeleton, controller_.matrices(), i == 5);
			matrices[i] = skeleton.bones[i].inv_transform * matrices[i];
			matrices[i] *= skeleton.root_transform;
		}
	}

	void update_bone(mhe::mat4x4& m, size_t index, const mhe::Skeleton& skeleton, const mhe::mat4x4* xforms, bool log)
	{
		const mhe::mat4x4& parent_transform = xforms[index];
		m *= parent_transform;
		if (skeleton.bones[index].parent_id != mhe::Bone::invalid_id)
			update_bone(m, skeleton.bones[index].parent_id, skeleton, xforms, log);
	}

	void load_animation(mhe::game::Engine& engine)
	{
		mhe::FilePath path = mhe::utils::path_join(engine.context().mesh_manager.path(), mhe::FilePath("rainbow_dash-default.anim"));
		std::ifstream stream(path.c_str(), std::ios::binary);
		if (!stream.is_open())
			return;
		AnimationLoader::load(animation_, stream);
		stream.close();
	}

	mhe::NodeInstance::IdType node_;
	mhe::fixed_size_allocator<true> allocator_;
	AnimationData animation_;
	AnimationController controller_;
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
	camera_parameters.znear = 0.1f;
	camera_parameters.zfar = 100.0f;
	mhe::game::FPSCameraController* camera_controller = new mhe::game::FPSCameraController(app.engine(), camera_parameters,
		mhe::vec3(0, 0, 1), mhe::vec3(0.0f, mhe::pi, 0));
	camera_controller->set_move_speed(5.0f);
	app.engine().scene().set_camera_controller(camera_controller);

	return app.run();
}
