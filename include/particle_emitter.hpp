#ifndef __PARTICLE_EMITTER_HPP__
#define __PARTICLE_EMITTER_HPP__

#include "particle.hpp"
#include <vector>

namespace mhe {

struct ParticleEmitterParameters
{
	cmn::uint min_emit_count;
	cmn::uint max_emit_count;
	cmn::uint min_emit_interval;
	cmn::uint max_emit_interval;
	float min_size;
	float max_size;
	float size_delta;
	colorf min_color;
	colorf max_color;
	colorf color_delta;
	v3d min_speed;
	v3d max_speed;
	v3d min_accel;
	v3d max_accel;
	cmn::uint min_lifetime;
	cmn::uint max_lifetime;
};

class ParticleEmitter
{
public:
	virtual ~ParticleEmitter() {}
	virtual void start(cmn::uint tick) = 0;
	virtual void stop() = 0;
	virtual std::vector<Particle> emit_particles(cmn::uint tick) = 0;
};

class DefaultParticleEmitter : public ParticleEmitter
{
public:
	DefaultParticleEmitter() :
		next_emit_(0), is_alive_(false)
	{}

	void init_with_parameters(const ParticleEmitterParameters& params)
	{
		params_ = params;
	}

	void start(cmn::uint tick);
	void stop();
	std::vector<Particle> emit_particles(cmn::uint tick);
private:
	ParticleEmitterParameters params_;
	cmn::uint next_emit_;
	bool is_alive_;
};

}

#endif
