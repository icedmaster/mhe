#include "particle.hpp"

namespace mhe
{
	// default constructor
	Particle::Particle() :
		begin_color_(cfWhite),
		end_color_(cfBlack),
		cur_color_(cfWhite),
		is_alive_(false),
		size_(0.1)
	{
        // default setup for vertexes
        v[0] = 0.0;   v[1] = 0.0;    v[2] = 0.0;
        v[3] = 0.0;   v[4] = size_;  v[5] = 0.0;
        v[6] = size_; v[7] = size_;  v[8] = 0.0;
        v[9] = size_; v[10] = 0.0;   v[11] = 0.0;
        // default setup for normals
        n[0] = 0.0; n[1] = 0.0;  n[2] = 1.0;
        n[3] = 0.0; n[4] = 0.0;  n[5] = 1.0;
        n[6] = 0.0; n[7] = 0.0;  n[8] = 1.0;
        n[9] = 0.0; n[10] = 0.0; n[11] = 1.0;
        // indexes
        i[0] = 0; i[1] = 1; i[2] = 2;
        i[3] = 2; i[4] = 3; i[5] = 0;
	}

	Particle::Particle(const Particle& p) :
		pos_(p.pos_),
		vel_(p.vel_),
		lifetime_(p.lifetime_),
		begin_color_(p.begin_color_),
		end_color_(p.end_color_),
		cur_color_(p.cur_color_),
		is_alive_(is_alive_),
		size_(0.1),
		v(p.v),
		n(p.n),
		i(p.i)
	{
	}

	void Particle::start(cmn::uint tick)
	{
		start_time_ = tick;
		is_alive_ = true;
	}

	void Particle::kill()
	{
		is_alive_ = false;
	}

	void Particle::draw_impl(const boost::shared_ptr<iDriver>& driver)
	{
		driver->draw(v.data(), n.data(), i.data(), i.size(), cur_color_);
	}

	void Particle::update_impl(cmn::uint tick)
	{
		if (!is_alive_) return;

		if ( lifetime_ && ((tick - start_time_) > lifetime_) )
		{
			is_alive_ = false;
			return;
		}
		// update buffers
        v[0] = pos_.x();         v[1] = pos_.y();          v[2] = pos_.z();
        v[3] = pos_.x();         v[4] = pos_.y() + size_;  v[5] = pos_.z();
        v[6] = pos_.x() + size_; v[7] = pos_.y() + size_;  v[8] = pos_.z();
        v[9] = pos_.x() + size_; v[10] = pos_.y();         v[11] = pos_.z();
        before_die_ = start_time_ + lifetime_ - tick;
	}
};
