#include "layer_scene_modifier.hpp"

namespace mhe {

namespace {

bool sort_node(Node* node1, Node* node2)
{
	return (node1->priority() < node2->priority());
}

}	// namespace

void LayerSceneModifier::apply(std::list< boost::shared_ptr<Node> >& nodes)
{
	nodes.sort(&sort_node);
}

}
