#include "batch_scene_modifier.hpp"

#include "node.hpp"

namespace mhe {

void BatchSceneModifier::apply(std::list< boost::shared_ptr<Node> >& nodes)
{
	std::list< boost::shared_ptr<Node> > batched;
	std::list< boost::shared_ptr<Node> >::iterator first = batched.end();
	for (std::list< boost::shared_ptr<Node> >::iterator it = nodes.begin();
		 it != nodes.end(); ++it)
	{
		const boost::shared_ptr<Node>& node = *it;
		bool update_first_node = node->is_batching_disabled();
		bool inserted = false;
		if (!update_first_node)
		{
			for (std::list< boost::shared_ptr<Node> >::iterator bit = first; bit != batched.end(); ++bit)
			{
				const Texture& other_texture = *(node->texture());
				if ((*bit)->texture()->is_equal(other_texture))
				{
					batched.insert(bit, node);
					inserted = true;
					break;
				}
			}
		}
		if (!inserted)
			batched.push_back(node);
		if (update_first_node)
			first = batched.end();
	}
}

}
