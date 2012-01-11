#include "circle_particle_effect.hpp"

namespace mhe
{
    void CircleParticleEffect::update_impl(std::vector<Particle>& p, cmn::uint tick)
    {
        if (!next_tick) return; // effect not started yet
        if (tick < next_tick) return;

        // update particle parameters
        if (need_to_upd_)
        {
            if (p.empty()) return;
            float old_radius = p[0].position().length();
            for (size_t i = 0; i < p.size(); ++i)
            {
                // cos(a) = x1 / l1;
                // x2 = l2 * cos(a) => x2 = l2 * x1 / l1;
                float nx = radius_ * p[i].position().x() / old_radius;
                float ny = radius_ * p[i].position().y() / old_radius;
                p[i].set_position(v3d(nx, ny, 0));
            }
        }

        for (size_t i = 0; i < p.size(); ++i)
        {
            // move partile
            v3d pos(p[i].position());
            float angle = speed_ * update_time_ / 1000.0;
            angle = deg_to_rad(angle);
            // cos(a + b) = cos(a)*cos(b)-sin(a)*sin(b)
            // sin(a + b) = sin(a)*cos(b)-sin(b)*cos(a)
            const float x1 = pos.x();
            const float y1 = pos.y();
            pos.set(x1 * cos(angle) - y1 * sin(angle),
                    y1 * cos(angle) + x1 * sin(angle), 0);
            // add random for y
            //float r = static_cast<float>(utils::srandom(5)) / 100.0;
            float r = 0.0;
            pos.set(pos.x(), pos.y() + r, pos.z());
            p[i].set_position(pos);

            colorf c = p[i].color();
            c.set_w(alpha_);
            p[i].set_color(c);
        }
        // add some blending
        alpha_ += alpha_increase_;
        if ( (alpha_ >= alpha_h_border_) || (alpha_ <= alpha_l_border_) )
            alpha_increase_ = -alpha_increase_;
        next_tick += update_time_;
    }

    void CircleParticleEffect::init_impl(std::vector<Particle>& p)
    {
        //cmn::uint lftime = 600;
        v3d pos;
        const float interval = 360.0 / static_cast<float>(p.size());
        for (size_t i = 0; i < p.size(); ++i)
        {
            p[i].set_color(c);
            p[i].set_lifetime(0);
            //p[i].set_velocity(v3d(x_pos, y_pos, 0));
            p[i].set_size(0.2);

            float rangle = deg_to_rad(next_particle_angle_);
            pos.set(radius_ * cos(rangle), radius_ * sin(rangle), 0.0);
            p[i].set_position(pos);
            next_particle_angle_ += interval;
        }
    }

    void CircleParticleEffect::start_impl(cmn::uint tick)
    {
        next_tick = tick + update_time_;
    }

    void CircleParticleEffect::setRadius(float radius)
    {
        need_to_upd_ = true;
        radius_ = radius;
    }
}
