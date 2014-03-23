#ifndef __NODE_COMPONENT_HPP__
#define __NODE_COMPONENT_HPP__

#include "component.hpp"
#include "node.hpp"
#include "sprite.hpp"
#include "message_transform.hpp"
#include "message_color.hpp"

namespace mhe {
namespace game {

class NodeComponent : public Node, public Component
{
	COMPONENT_METHODS(NodeComponent, "node");
public:
	NodeComponent(const std::string& name) :
		Component(name)
	{}

	virtual ~NodeComponent() {}
private:
	void do_subscribe(Component* parent);
	bool update_impl(const Message& message);

	mat4x4 transform_impl() const
	{
		return Node::transform();
	}

	void process_transform_event(const TransformMessage& message);
	void process_enable_event(const EnableMessage& message);
	void process_color_event(const ColorMessage& message);
};

class SpriteComponent : public SpriteBase, public NodeComponent
{
	COMPONENT_METHODS(SpriteComponent, "sprite");
public:
	SpriteComponent(const std::string& name) :
		NodeComponent(name)
	{}
private:
	void set_mesh_impl(const mesh_ptr& mesh)
	{
		NodeComponent::set_mesh(mesh);
	}

	texture_ptr texture_impl() const
	{
		return NodeComponent::texture();
	}

	void set_transform_impl(const matrixf& m)
	{
		NodeComponent::set_transform(m);
	}

	void rebuild_impl()
	{
		NodeComponent::rebuild();
	}

	size_t materials_number_impl() const
	{
		return NodeComponent::materials_number();
	}

	mesh_ptr mesh_impl() const
	{
		return NodeComponent::mesh();
	}
private:
	void on_material_changed()
	{
		SpriteBase::update_buffers();
	}
};

}}

#endif
