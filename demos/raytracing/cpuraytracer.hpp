#ifndef __CPURAYTRACER_HPP__
#define __CPURAYTRACER_HPP__

#include <math/ray.hpp>

namespace mhe {
	class Camera;
	class Texture;

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
	void kick_draw_call();

	mhe::game::Engine* engine_;
	mhe::Texture* texture_;
};

#endif