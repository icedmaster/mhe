#ifndef __NODE_COMPONENT_HPP__
#define __NODE_COMPONENT_HPP__

#include "component.hpp"
#include "node.hpp"
#include "scene.hpp"
#include "message_transform.hpp"

namespace mhe {
namespace game {

class NodeComponent : public Component
{
public:
	NodeComponent(const std::string& name, const std::string& add_name,
				  Node* node, const boost::shared_ptr<Scene>& scene,
				  bool add_to_scene = true):
		Component(name, add_name),
		node_(node), scene_(scene), at_scene_(add_to_scene)
	{
		if (add_to_scene)
			scene->add(node_);		
	}

	~NodeComponent()
	{
		scene_->remove(node_);
	}
private:
	void do_subscribe(Component* parent);
	bool update_impl(const Message& message);
	void process_transform_event(const TransformMessage& message);
	void process_enable_event(const EnableMessage& message);

	nodeptr node_;
	boost::shared_ptr<Scene> scene_;
	bool at_scene_;
};

}}

#endif
