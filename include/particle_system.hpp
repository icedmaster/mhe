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

	void set_texture(boost::shared_ptr<Texture> texture)
	{
		texture_ = texture;
	}

	bool is_alive() const;
private:
	void draw_impl(boost::shared_ptr<Driver> driver);
	void update_impl(cmn::uint tick);
	void start_impl(cmn::uint tick);
private:
	std::list<Particle> particles_;
	boost::shared_ptr<ParticleEmitter> emitter_;          
	boost::shared_ptr<Texture> texture_;
	std::vector<float> vbuf;
	std::vector<float> nbuf;
	std::vector<cmn::uint> ibuf;
	std::vector<float> cbuf;
	std::vector<float> tbuf;
};

}

#endif
