#ifndef __CPURAYTRACER_HPP__
#define __CPURAYTRACER_HPP__

#include <math/ray.hpp>
#include <math/matrix.hpp>
#include <core/fixed_size_vector.hpp>
#include <threads/thread.hpp>

namespace mhe {
	class Camera;
	class Texture;
	struct LightInstance;

namespace game {
	class Engine;
}}

enum Pass
{
	pass_default,
	pass_shadow
};

struct TraceContext
{
	size_t node_id;
	float shade_term;
	mhe::vec3 emission;
	Pass pass;
	mhe::vec3* lights_directions;
	mhe::vec3* lights_diffuse_colors;
	size_t lights_number;
};

class CPURaytracer
{
public:
	void init(mhe::game::Engine* engine);

	void render(const mhe::Camera& camera);
private:
	struct ThreadConfig
	{
		uint8_t* data;
		int xs;
		int ys;
		int width;
		int height;
		int total_width;
		int total_height;
		mhe::vec3 camera_position;
		mhe::mat4x4 inv_vp;
		mhe::vec3* lights_directions;
		mhe::vec3* diffuse_colors;
		size_t lights_number;
	};

	class TraceThread : public mhe::thread
	{
	public:
		void init(mhe::game::Engine* engine, const mhe::mat4x4* transforms,
			const mhe::mat4x4* inv_transforms, const ThreadConfig& thread_config);

		mhe::condition_variable& condition()
		{
			return cond_;
		}

		mhe::condition_variable& result_condition()
		{
			return res_cond_;
		}
	private:
		bool start_impl() override;
		void process_impl() override;

		mhe::game::Engine* engine_;
		ThreadConfig thread_config_;
		const mhe::mat4x4* transforms_;
		const mhe::mat4x4* inv_transforms_;
		mhe::condition_variable cond_;
		mhe::condition_variable res_cond_;
	};

	void kick_draw_call();

	mhe::game::Engine* engine_;
	mhe::Texture* texture_;
	mhe::fixed_capacity_vector<TraceThread, 16> threads_;
};

#endif