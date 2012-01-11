#include "particle_system.hpp"

namespace mhe
{
    ParticleSystem::ParticleSystem(cmn::uint num) :
        autorestart_(false),
        alive_(true),
        vbuf(0), nbuf(0), ibuf(0), cbuf(0), tbuf(0)
    {
        init_particles(num);
    }

    ParticleSystem::ParticleSystem(cmn::uint num, const v3d& pos, bool autorestart) :
        autorestart_(autorestart),
        alive_(true),
        vbuf(0), nbuf(0), ibuf(0), cbuf(0), tbuf(0)
    {
        init_particles(num);
        // setup transformation
        Transform::translate(pos);
    }

    void ParticleSystem::init_particles(cmn::uint num)
    {
        particles_.reserve(num);
        // push particles into buffer
        for (cmn::uint i = 0; i < num; ++i)
        {
            Particle p;
            particles_.push_back(p);
        }
        // reserve space for buffers
        vbuf.reserve(particle_vertexes_num * particles_.size());
        nbuf.reserve(particle_vertexes_num * particles_.size());
        ibuf.reserve(particle_indices_num * particles_.size());
        cbuf.reserve(4 * particles_.size());
    }

    void ParticleSystem::draw_impl(const boost::shared_ptr<iDriver>& driver)
    {
        vbuf.clear();
        nbuf.clear();
        ibuf.clear();
        cbuf.clear();
        tbuf.clear();
        // assign particles data to buffers
        for (size_t it = 0; it < particles_.size(); ++it)
        {
            if (!particles_[it].alive()) continue;
            // save current vertex vector size - we need append it for
            // next indices
            const size_t v_sz = vbuf.size() / 3;
            vbuf.insert(vbuf.end(), particles_[it].get_vertexes().begin(),
                                    particles_[it].get_vertexes().end());
            nbuf.insert(nbuf.end(), particles_[it].get_normals().begin(),
                                    particles_[it].get_normals().end());

            // recalculate indices for each particle
            const boost::array< cmn::uint, particle_indices_num>& a =
            particles_[it].get_indices();
            for (size_t j = 0; j < a.size(); ++j)
                ibuf.push_back(v_sz + a[j]);
            // fill color for each vertex
            for (cmn::uint j = 0; j < particle_vertexes_num; ++j)
                cbuf.insert(cbuf.end(), particles_[it].color().get(),
                                        particles_[it].color().get() + 4);
            //particles_[i]->draw(driver);
            // init texture coordinates (for all particles they are the same)
            if (texture_)
            {
                tbuf.push_back(0.0); tbuf.push_back(0.0);
                tbuf.push_back(0.0); tbuf.push_back(1.0);
                tbuf.push_back(1.0); tbuf.push_back(1.0);
                tbuf.push_back(1.0); tbuf.push_back(0.0);
            }
        }

        driver->mask_zbuffer();
        driver->enable_blending(ALPHA_ONE_MINUS_ALPHA);

        // setup texture pointer
        float* tp = (texture_) ? &tbuf[0] : 0;
        if (texture_)
            texture_->prepare();
        driver->draw(Transform::get_transform(), &vbuf[0], &nbuf[0], tp, &cbuf[0], &ibuf[0], ibuf.size());
        if (texture_)
            texture_->clean();
        driver->disable_blending();
        driver->unmask_zbuffer();
    }

    void ParticleSystem::update_impl(cmn::uint tick)
    {
		// check for dead particles
        alive_ = false;
		for (std::vector<Particle>::iterator it = particles_.begin();
											 it != particles_.end();
											 ++it)
		{
			if (!(it->alive()))
			{
			    if (!autorestart_)
			    {
                    continue;   // process next particle
			    }
                else
                {
                    // change position of particle to default
                    it->set_position(v3d());
                    // and restart it
                    it->start(tick);
                }
			}
			it->update(tick);
			alive_ = true;
		}

		if (!alive_)
		{
            particles_.clear();
		    return;
		}

        effect_->update(particles_, tick);
    }

    void ParticleSystem::addEffect(ParticleEffect* pe)
    {
        effect_.reset(pe);
        pe->init(particles_);
    }

    void ParticleSystem::addEffect(const boost::shared_ptr<ParticleEffect>& pe)
    {
        effect_ = pe;
        effect_->init(particles_);
    }

    void ParticleSystem::start_impl(cmn::uint tick)
    {
        //
        for (size_t i = 0; i < particles_.size(); ++i)
            particles_[i].start(tick);
        // start effects
        effect_->start(tick);
    }

    void ParticleSystem::setSize(float size)
    {
        for (size_t i = 0; i < particles_.size(); ++i)
            particles_[i].set_size(size);
    }

    void ParticleSystem::setTexture(const boost::shared_ptr<iTexture>& t)
    {
        texture_ = t;
        const size_t tsize = (particle_vertexes_num / 3) * 2 * particles_.size();
        tbuf.reserve(tsize);
    }
}
