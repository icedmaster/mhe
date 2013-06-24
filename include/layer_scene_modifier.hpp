#ifndef __LAYER_SCENE_MODIFIER_HPP__
#define __LAYER_SCENE_MODIFIER_HPP__

#include "scene_modifier.hpp"

namespace mhe {

class LayerSceneModifier : public SceneModifier
{
public:
	LayerSceneModifier(const std::string& name = "layer") :
		SceneModifier(name)
	{}
	void apply(nodelist& nodes);
	SceneModifier::UpdateMode update_mode() const
	{
		return node_add;
	}
};

}

#endif
