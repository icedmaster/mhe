#include "particle_emitter.hpp"
#include "utils/sysutils.hpp"

namespace mhe
{

void DefaultParticleEmitter::start(cmn::uint /*tick*/)
{
	is_alive_ = true;
}

void DefaultParticleEmitter::stop()
{
	is_alive_ = false;
}

std::vector<Particle> DefaultParticleEmitter::emit_particles(cmn::uint tick)
{
	if (!is_alive_) return std::vector<Particle>();
	if (tick < next_emit_) return std::vector<Particle>();
	// particles number
	cmn::uint number = utils::range_random(params_.min_emit_count, params_.max_emit_count);
	std::vector<Particle> particles;
	particles.reserve(number);
	// init particles parameters
	for (cmn::uint i = 0; i < number; ++i)
	{
		float size = utils::range_random(params_.min_size, params_.max_size);
		colorf color(utils::range_random(params_.min_color.r(), params_.max_color.r()),
					 utils::range_random(params_.min_color.g(), params_.max_color.g()),
					 utils::range_random(params_.min_color.b(), params_.max_color.b()),
					 utils::range_random(params_.min_color.a(), params_.max_color.a()));
		v3d speed(utils::range_random(params_.min_speed.x(), params_.max_speed.x()),
				  utils::range_random(params_.min_speed.y(), params_.max_speed.y()),
				  utils::range_random(params_.min_speed.z(), params_.max_speed.z()));
		v3d accel(utils::range_random(params_.min_accel.x(), params_.max_accel.x()),
				  utils::range_random(params_.min_accel.y(), params_.max_accel.y()),
				  utils::range_random(params_.min_accel.z(), params_.max_accel.z()));
		cmn::uint lifetime = utils::range_random(params_.min_lifetime, params_.max_lifetime);
		particles.push_back(Particle(size, params_.size_delta, color, params_.color_delta,
									 params_.color_delta.a(),
									 speed, accel, lifetime));
	}
	cmn::uint emit_delta = utils::range_random(params_.min_emit_interval, params_.max_emit_interval);
	next_emit_ = tick + emit_delta;
	return particles;
}

}
