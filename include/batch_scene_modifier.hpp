#ifndef __BATCH_SCENE_MODIFIER_HPP__
#define __BATCH_SCENE_MODIFIER_HPP__

#include "scene_modifier.hpp"

namespace mhe {

class BatchSceneModifier : public SceneModifier
{
public:
	BatchSceneModifier(const std::string& name = "batch") :
		SceneModifier(name)
	{}

	void apply(std::list< boost::shared_ptr<Node> >& nodes);
	SceneModifier::UpdateMode update_mode() const;
};

}

#endif
