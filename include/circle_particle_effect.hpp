#ifndef _CIRCLE_PARTICLE_EFFECT_HPP_
#define _CIRCLE_PARTICLE_EFFECT_HPP_

#include "particle_effect.hpp"

namespace mhe
{
    class CircleParticleEffect : public ParticleEffect
    {
        private:
            colorf c;
            cmn::uint next_tick;
            float radius_;
            float speed_;
            float alpha_;
            float alpha_increase_;
            float alpha_l_border_;
            float alpha_h_border_;
            float next_particle_angle_;
            bool need_to_upd_;
            cmn::uint update_time_;
        private:
            void update_impl(std::vector<Particle>&, cmn::uint);
            void set_update_time_impl(cmn::uint ticks)
            {
                update_time_ = ticks;
            }
            void init_impl(std::vector<Particle>& p);
            void start_impl(cmn::uint tick);
        public:
            // speed in degrees per second
            CircleParticleEffect(const colorf& color = cfWhite, float radius = 1.0,
                                 float speed = 10.0) :
                c(color),
                next_tick(0),
                radius_(radius),
                speed_(speed),
                alpha_(0.0),
                alpha_increase_(0.01),
                alpha_l_border_(0.0),
                alpha_h_border_(1.0),
                next_particle_angle_(0.0),
                need_to_upd_(false),
                update_time_(10)
            {
            }

            void setRadius(float radius);

            void setAlphaMode(float alpha, float increase = 0.01,
                              float lborder = 0.0, float hborder = 1.0)
            {
                alpha_ = alpha;
                alpha_increase_ = increase;
                alpha_l_border_ = lborder;
                alpha_h_border_ = hborder;
            }

            void setSpeed(float speed)
            {
                speed_ = speed;
            }
        };
}

#endif
