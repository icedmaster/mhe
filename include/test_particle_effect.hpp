#ifndef _TEST_PARTICLE_EFFECT_HPP_
#define _TEST_PARTICLE_EFFECT_HPP_

#include "particle_effect.hpp"

namespace mhe
{
    namespace test
    {
        class TestParticleEffect : public ParticleEffect
        {
            private:
                colorf c;
                cmn::uint next_tick;
            private:
                void update_impl(std::vector<Particle>&, cmn::uint);
                void set_update_time_impl(cmn::uint) {}
                void init_impl(std::vector<Particle>& p);
                void start_impl(cmn::uint tick);
            public:
                TestParticleEffect(const colorf& color) :
                    c(color),
                    next_tick(0)
                {
                }


        };
    }
}

#endif
