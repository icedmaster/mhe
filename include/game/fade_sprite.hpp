#ifndef __FADE_SPRITE_HPP__
#define __FADE_SPRITE_HPP__

#include "sprite.hpp"

namespace mhe {
namespace game {

class FadeSprite : public iNode
{
public:
	FadeSprite(const rect<float>& geom, cmn::uint fade_time, 
			   const colorf& color = cfBlack);
private:
	rect<float> geom_;
	cmn::uint fade_time_;
	float alpha_;
	colorf color_;
	cmn::uint prev_tick_;
private:
	virtual void draw_impl(const boost::shared_ptr<iDriver>& driver);
	virtual void update_impl(cmn::uint tick);
	virtual int get_priority() const   {return 5;}	// high priority
};

}}	// namespaces

#endif	// __FADE_SPRITE_HPP__
