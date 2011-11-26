#ifndef _PARTICLE_EFFECT_HPP_
#define _PARTICLE_EFFECT_HPP_

#include "particle.hpp"
#include "utils/sysutils.hpp"
#include <vector>

namespace mhe
{
    class ParticleEffect
    {
        private:
            virtual void update_impl(std::vector<Particle>&, cmn::uint) {}
            virtual void set_update_time_impl(cmn::uint) {}
            virtual void init_impl(std::vector<Particle>&) {}
            virtual void start_impl(cmn::uint) {}
        public:
            virtual ~ParticleEffect() {}
            void update(std::vector<Particle>& p, cmn::uint tick)
            {
                update_impl(p, tick);
            }

            void set_update_time(cmn::uint interval)
            {
                set_update_time_impl(interval);
            }

            void init(std::vector<Particle>& p)
            {
                init_impl(p);
            }

            void start(cmn::uint tick)
            {
                start_impl(tick);
            }
    };
}

#endif
