#ifndef __CPURAYTRACER_HPP__
#define __CPURAYTRACER_HPP__

#include <math/ray.hpp>

namespace mhe {
	class Camera;
	class Texture;
	struct LightInstance;

namespace game {
	class Engine;
}}

class CPURaytracer
{
public:
	void init(mhe::game::Engine* engine);

	void render(const mhe::Camera& camera);
private:
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
	};

	mhe::vec3 trace(TraceContext& trace_context, const mhe::rayf& r, const mhe::mat4x4* transforms,
		const mhe::mat4x4* inv_transforms, size_t bounce);
	mhe::vec3 lit_pixel(const mhe::vec3& pos, const mhe::vec3& nrm,
		const mhe::LightInstance& light_instance, const mhe::vec3& light_direction) const;
	mhe::vec3 lit_pixel(const mhe::vec3& pos, const mhe::vec3& nrm,
		const mhe::vec3& direction, const mhe::vec3& color) const;
	void kick_draw_call();

	mhe::game::Engine* engine_;
	mhe::Texture* texture_;
};

#endif