#ifndef _PARTICLE_HPP_
#define _PARTICLE_HPP_

#include "mhe_math.hpp"
#include "types.hpp"
#include "video_driver.hpp"
#include <boost/array.hpp>

namespace mhe
{
const size_t particle_vertexes_num = 12;
const size_t particle_indices_num  = 6;

class Particle
{
	static const cmn::uint update_interval = 5;
public:
	Particle(float size, float size_delta,
			 const colorf& color, const colorf& color_delta, float alpha_delta,
			 const v3d& speed, const v3d& accel, cmn::uint lifetime) :
		size_(size), size_delta_(size_delta),
		color_(color), color_delta_(color_delta), alpha_delta_(alpha_delta),
		speed_(speed), accel_(accel), 
		start_time_(0), lifetime_(lifetime)
	{}

	const v3d& position() const 
	{
		return position_;
	}

	const colorf& color() const
	{
		return color_;
	}

	bool is_alive(cmn::uint tick) const
	{
		if (!start_time_) return true;
		return ((tick - start_time_) < lifetime_);
	}

	float size() const
	{
		return size_;
	}

	void update(cmn::uint tick)
	{
		if (!start_time_) start_time_ = tick;
		if (!is_alive(tick)) return;
		if (tick < next_tick_) return;
		size_ += size_delta_;
		color_ += color_delta_;
		color_.set_w(color_.a() + alpha_delta_);
		position_ += speed_;
		speed_ *= accel_;
		next_tick_ = tick + update_interval;
	}
private:
	float size_;
	float size_delta_;
	colorf color_;
	colorf color_delta_;
	float alpha_delta_;
	v3d speed_;
	v3d accel_;
	v3d position_;
	v3d direction_;
	cmn::uint start_time_;
	cmn::uint lifetime_;
	// update parameters
	cmn::uint next_tick_;
};

}

#endif
