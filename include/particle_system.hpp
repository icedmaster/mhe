#ifndef _PARTICLE_SYSTEM_HPP_
#define _PARTICLE_SYSTEM_HPP_

#include "particle_emitter.hpp"
#include "mhe_math.hpp"
#include "texture.hpp"
#include "node.hpp"
#include <boost/shared_ptr.hpp>
#include <list>

namespace mhe
{

class ParticleSystem : public Node
{
public:
	ParticleSystem()
	{}

	ParticleSystem(const v3d& pos)
	{
		matrixf m;
		m.set_translate(pos);
		Transform::set_transform(m);
	}

	void set_emitter(boost::shared_ptr<ParticleEmitter> emitter)
	{
		emitter_ = emitter;
	}

	//bool is_alive() const;
private:
	void draw_impl(Context& context);
	void update_impl(cmn::uint tick);
	void start_impl(cmn::uint tick);
private:
	std::list<Particle> particles_;
	boost::shared_ptr<ParticleEmitter> emitter_;          
};

}

#endif
