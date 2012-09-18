#include "layer_scene_modifier.hpp"

#include "node.hpp"

namespace mhe {

namespace {

bool sort_node(boost::shared_ptr<Node> node1, boost::shared_ptr<Node> node2)
{
	return (node1->priority() < node2->priority());
}

}	// namespace

void LayerSceneModifier::apply(std::list< boost::shared_ptr<Node> >& nodes)
{
	nodes.sort(&sort_node);
}

}
