#include "particle_emitter.hpp"
#include "utils/sysutils.hpp"

namespace mhe
{

void PointParticleEmitter::start(cmn::uint /*tick*/)
{
	is_alive_ = true;
}

void PointParticleEmitter::stop()
{
	is_alive_ = false;
}

std::vector<Particle> PointParticleEmitter::emit_particles(cmn::uint tick)
{
	if (!is_alive_) return std::vector<Particle>();
	if (tick < next_emit_) return std::vector<Particle>();
	// particles number
	cmn::uint number = params_.emit_count.get_random();
	std::vector<Particle> particles;
	particles.reserve(number);
	// init particles parameters
	for (cmn::uint i = 0; i < number; ++i)
	{
		cmn::uint lifetime = params_.lifetime.get_random();
		float div = lifetime / Particle::update_interval;
		float size = params_.size.get_random();
		float size_delta = (params_.end_size - size) / div;
		const colorf& color = params_.color.get_random();
		colorf color_delta = (params_.end_color - color) / div;
		float spread = deg_to_rad(params_.spread.get_random());
		const v3d& speed = params_.speed.get_random();
		v3d particle_speed = (!params_.spread.high() && !params_.spread.low()) ?
			speed : v3d(speed.x() * cosf(spread),
					 	speed.y() * sinf(spread),
						speed.z());		
		const v3d& accel = params_.accel.get_random();
		float angle = deg_to_rad(params_.angle.get_random());
		particles.push_back(Particle(size, size_delta, color, color_delta,
									 color_delta.a(),
									 particle_speed, accel, angle, lifetime));
	}
	cmn::uint emit_delta = params_.emit_interval.get_random();
	next_emit_ = tick + emit_delta;
	return particles;
}

}
