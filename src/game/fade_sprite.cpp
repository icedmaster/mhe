#include "game/fade_sprite.hpp"

namespace mhe {
namespace game {

FadeSprite::FadeSprite(const rect<float>& geom, cmn::uint fade_time, 
					   const colorf& color) :
	geom_(geom), fade_time_(fade_time), alpha_(0.0), color_(color),
	prev_tick_(0)
{
}

void FadeSprite::draw_impl(const boost::shared_ptr<iDriver>& driver)
{
	const float v[] = {geom_.ll().x(), geom_.ll().y(), 0.0,
					   geom_.ll().x(), geom_.rh().y(), 0.0,
					   geom_.rh().x(), geom_.rh().y(), 0.0,
					   geom_.rh().x(), geom_.ll().x(), 0.0};
	const float c[] = {color_.r(), color_.g(), color_.b(), alpha_,
					   color_.r(), color_.g(), color_.b(), alpha_,
					   color_.r(), color_.g(), color_.b(), alpha_,
					   color_.r(), color_.g(), color_.b(), alpha_};
	const cmn::uint i[] = {0, 1, 2, 2, 3, 0};

	driver->mask_zbuffer();
	driver->enable_blending(ALPHA_ONE_MINUS_ALPHA);

	driver->draw(v, 0, 0, c, i, 6);

	driver->disable_blending();
	driver->unmask_zbuffer();
}

void FadeSprite::update_impl(cmn::uint tick)
{
	if (alpha_ >= 1.0) return;
	if (!prev_tick_)
	{
		prev_tick_ = tick;
		return;
	}
	float delta = static_cast<float>(tick - prev_tick_) /
		static_cast<float>(fade_time_);
	alpha_ += delta;
	prev_tick_ = tick;
}

}}
