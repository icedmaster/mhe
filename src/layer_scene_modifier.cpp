#include "layer_scene_modifier.hpp"

#include <algorithm>
#include "node.hpp"

namespace mhe {

namespace {

bool sort_node(boost::shared_ptr<Node> node1, boost::shared_ptr<Node> node2)
{
	return (node1->priority() < node2->priority());
}

}	// namespace

void LayerSceneModifier::apply(nodelist& nodes)
{
	if (nodes.empty()) return;
	std::sort(nodes.begin(), nodes.end(), &sort_node);
	nodelist::iterator first = nodes.begin();
	++first;
	nodes.front()->set_batching_enabled();
	for (nodelist::iterator it = first; it != nodes.end();
		 ++it)
	{
		const boost::shared_ptr<Node>& current_node = *it;
		nodelist::iterator prev_it = it;
		--prev_it;
		const boost::shared_ptr<Node>& prev_node = *prev_it;		
		if (current_node->priority() != prev_node->priority())
			current_node->set_batching_disabled();
		else current_node->set_batching_enabled();
	}
}

}
