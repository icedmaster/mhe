#include "test_particle_effect.hpp"

namespace mhe {
namespace test {

    void TestParticleEffect::update_impl(std::vector<Particle>& p, cmn::uint tick)
    {
        if (!next_tick) return; // effect not started yet
        if (tick < next_tick) return;
        for (size_t i = 0; i < p.size(); ++i)
        {
            // update position
            v3d pos = p[i].position();
            pos += p[i].velocity();
            float tmp = static_cast<float>(utils::srandom(100, 80)) / 500.0;
            pos.set(pos.x() + tmp, pos.y(), pos.z());
            p[i].set_position(pos);
            // update particle color
            colorf color = p[i].color();
            float t = static_cast<float>(p[i].before_die()) /
                      static_cast<float>(p[i].lifetime());
            color.set_w(t);
            p[i].set_color(color);
        }
        next_tick += 10;
    }

    void TestParticleEffect::init_impl(std::vector<Particle>& p)
    {
        float y_pos = 0.02;
        float x_pos = -0.01;
        cmn::uint lftime = 600;
        for (size_t i = 0; i < p.size(); ++i)
        {
            p[i].set_color(c);
            p[i].set_lifetime(lftime);
            p[i].set_velocity(v3d(x_pos, y_pos, 0));
            p[i].set_size(0.05);
            y_pos += 0.02;
            //x_pos += 0.1;
            x_pos = -x_pos;
            //lftime += 500;
        }
    }

    void TestParticleEffect::start_impl(cmn::uint tick)
    {
        next_tick = tick + 10;
    }

}
}
