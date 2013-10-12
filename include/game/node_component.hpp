#ifndef __NODE_COMPONENT_HPP__
#define __NODE_COMPONENT_HPP__

#include "component.hpp"
#include "node.hpp"
#include "scene.hpp"
#include "message_transform.hpp"
#include "message_color.hpp"

namespace mhe {
namespace game {

class NodeComponent : public Component
{
public:
	NodeComponent(const std::string& name,
				  Node* node, const boost::shared_ptr<Scene>& scene,
				  bool add_to_scene = true):
		Component(name),
		node_(node), scene_(scene), at_scene_(add_to_scene)
	{
		if (add_to_scene)
			scene->add(node_);		
	}

	~NodeComponent()
	{
		scene_->remove(node_);
	}

	nodeptr node() const
	{
		return node_;
	}
private:
	void do_subscribe(Component* parent);
	bool update_impl(const Message& message);

	std::string add_name_impl() const
	{
		return "node";
	}

	void process_transform_event(const TransformMessage& message);
	void process_enable_event(const EnableMessage& message);
	void process_color_event(const ColorMessage& message);

	nodeptr node_;
	boost::shared_ptr<Scene> scene_;
	bool at_scene_;
};

}}

#endif
