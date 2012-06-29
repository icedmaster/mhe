#include "particle_system.hpp"

namespace mhe
{
void ParticleSystem::draw_impl(const Context& context)
{
	std::vector<float> vbuf;
	std::vector<float> nbuf;
	std::vector<cmn::uint> ibuf;
	std::vector<float> cbuf;
	std::vector<float> tbuf;
	const float n[12] = {0.0, 0.0, 1.0,
						 0.0, 0.0, 1.0,
						 0.0, 0.0, 1.0, 
						 0.0, 0.0, 1.0};
	const cmn::uint i[6] = {0, 1, 2, 2, 3, 0};
	boost::shared_ptr<Texture> tex = texture();
	// assign particles data to buffers
	for (std::list<Particle>::iterator it = particles_.begin(); it != particles_.end(); ++it)
	{
		const Particle& p = *it;
		// save current vertex vector size - we need append it for
		// next indices
		const size_t v_sz = vbuf.size() / 3;
		// calculate vertexes
		const v3d& pos = p.position();
		float v[12] = {pos.x(), pos.y(), pos.z(),
					   pos.x(), pos.y() + p.size(), pos.z(),
					   pos.x() + p.size(), pos.y() + p.size(), pos.z(),
					   pos.x() + p.size(), pos.y(), pos.z()};
		vbuf.insert(vbuf.end(), v, v + 12);
		nbuf.insert(nbuf.end(), n, n + 12);

		// recalculate indices for each particle
		for (size_t j = 0; j < 6; ++j)
			ibuf.push_back(v_sz + i[j]);
		// fill color for each vertex
		for (cmn::uint j = 0; j < 4; ++j)
			cbuf.insert(cbuf.end(), p.color().get(),
						p.color().get() + 4);
		// init texture coordinates (for all particles they are the same)
		if (tex != nullptr)
		{
			tbuf.push_back(0.0); tbuf.push_back(0.0);
			tbuf.push_back(0.0); tbuf.push_back(1.0);
			tbuf.push_back(1.0); tbuf.push_back(1.0);
			tbuf.push_back(1.0); tbuf.push_back(0.0);
		}
	}

	boost::shared_ptr<Driver> driver = context.driver();

	driver->mask_zbuffer();
	driver->enable_blending(ALPHA_ONE_MINUS_ALPHA);

	// setup texture pointer
	float* tp = (tex != nullptr) ? &tbuf[0] : 0;
	if (tex != nullptr)
		tex->prepare();
	driver->draw(Transform::get_transform(), &vbuf[0], &nbuf[0], tp, &cbuf[0], &ibuf[0], ibuf.size());
	if (tex != nullptr)
		tex->clean();
	driver->disable_blending();
	driver->unmask_zbuffer();
}

void ParticleSystem::update_impl(cmn::uint tick)
{
	// check for dead particles
	for (std::list<Particle>::iterator it = particles_.begin(); it != particles_.end();)
	{
		if (!(it->is_alive(tick)))
			it = particles_.erase(it);
		else
		{
			it->update(tick);
			++it;
		}
	}
	// generate new particles
	const std::vector<Particle>& emitted = emitter_->emit_particles(tick);
	particles_.insert(particles_.end(), emitted.begin(), emitted.end());
}

void ParticleSystem::start_impl(cmn::uint tick)
{
	emitter_->start(tick);
}
}
