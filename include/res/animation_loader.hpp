#ifndef __ANIMATION_LOADER_HPP__
#define __ANIMATION_LOADER_HPP__

#include "render/animation.hpp"

namespace mhe {

struct Context;

struct AnimationLoader
{
	typedef AnimationData type;
	typedef SkeletalAnimationInstance instance_type;
	typedef Context context_type;
	static MHE_EXPORT bool load(type& res, const std::string& name, const context_type* context);
};

}

#endif
