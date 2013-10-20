#ifndef __PARTICLE_EMITTER_HPP__
#define __PARTICLE_EMITTER_HPP__

#include "particle.hpp"
#include <vector>

namespace mhe {

struct ParticleEmitterParameters
{
	range<cmn::uint> emit_count;
	range<cmn::uint> emit_interval;
	range<float> size;
	float end_size;
	range<colorf> color;
	colorf end_color;
	range<v3d> speed;
	range<float> spread;
	range<v3d> accel;
	range<float> angle;
	range<cmn::uint> lifetime;
};

class ParticleEmitter
{
public:
	virtual ~ParticleEmitter() {}
	virtual void start(cmn::uint tick) = 0;
	virtual void stop() = 0;
	virtual void init_with_parameters(const ParticleEmitterParameters& params) = 0;
	virtual ParticleEmitterParameters& parameters() = 0;
	virtual std::vector<Particle> emit_particles(cmn::uint tick) = 0;
};

class PointParticleEmitter : public ParticleEmitter
{
public:
	PointParticleEmitter() :
		next_emit_(0), is_alive_(false)
	{}

	void init_with_parameters(const ParticleEmitterParameters& params)
	{
		params_ = params;
	}

	ParticleEmitterParameters& parameters()
	{
		return params_;
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
