#ifndef __BATCH_SCENE_MODIFIER_HPP__
#define __BATCH_SCENE_MODIFIER_HPP__

#include "scene_modifier.hpp"

namespace mhe {

class BatchSceneModifier : public SceneModifier
{
public:
	void apply(std::list< boost::shared_ptr<Node> >& nodes);
};

}

#endif
