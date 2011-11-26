#ifndef _PARTICLE_SYSTEM_HPP_
#define _PARTICLE_SYSTEM_HPP_

#include "particle.hpp"
#include "particle_effect.hpp"
#include "mhe_math.hpp"
#include "itexture.hpp"
#include <boost/shared_ptr.hpp>

namespace mhe
{
    class ParticleSystem : public iNode
    {
        private:
            std::vector<Particle> particles_;
            boost::shared_ptr<ParticleEffect> effect_;            
            bool autorestart_;
            bool alive_;
            boost::shared_ptr<iTexture> texture_;
            std::vector<float> vbuf;
            std::vector<float> nbuf;
            std::vector<cmn::uint> ibuf;
            std::vector<float> cbuf;
            std::vector<float> tbuf;

            void init_particles(cmn::uint num);
        private:
            void draw_impl(const boost::shared_ptr<iDriver>& driver);
            void update_impl(cmn::uint tick);
            bool is_alive() const
            {
                return alive_;
            }

            void start_impl(cmn::uint tick);
        public:
            ParticleSystem(cmn::uint num);
            ParticleSystem(cmn::uint num, const v3d& pos, bool autorestart = false);

            void addEffect(ParticleEffect* pe);
            void addEffect(const boost::shared_ptr<ParticleEffect>& pe);

            // particles modification
            void setSize(float size);

            void setTexture(const boost::shared_ptr<iTexture>& t);
    };
};

#endif
