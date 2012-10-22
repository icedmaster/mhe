#ifndef __NODE_COMPONENT_HPP__
#define __NODE_COMPONENT_HPP__

#include "component.hpp"
#include "node.hpp"
#include "scene.hpp"

namespace mhe {
namespace game {

class NodeComponent : public Component
{
public:
	NodeComponent(const std::string& name, const std::string& add_name,
				  Node* node, const boost::shared_ptr<Scene>& scene):
		Component(name, add_name),
		node_(node), scene_(scene)
	{
		scene->add(node_);
	}

	~NodeComponent()
	{
		scene_->remove(node_);
	}
private:
	void do_subscribe(Component* parent);
	bool update_impl(const Message& message);

	nodeptr node_;
	boost::shared_ptr<Scene> scene_;
};

}}

#endif
