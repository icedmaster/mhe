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
	mhe::vec3 trace(const mhe::rayf& r);
	mhe::vec3 lit_pixel(const mhe::vec3& pos, const mhe::vec3& nrm, const mhe::vec3& ambient,
		const mhe::LightInstance* light_sources, size_t lights_number) const;
	void kick_draw_call();

	mhe::game::Engine* engine_;
	mhe::Texture* texture_;
};

#endif