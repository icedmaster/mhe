#include "node.hpp"

#include "utils/sysutils.hpp"

namespace mhe {

Node::Node() : 
	Renderable(),
	priority_(priority_normal),
	parent_(nullptr),
	flags_(visible | alive),
	dirty_(false)
{
}

Node::Node(size_t vertexes_number, size_t indexes_number, size_t texcoord_number) :
	Renderable(vertexes_number, indexes_number, texcoord_number),
	priority_(priority_normal),
	parent_(nullptr),
	flags_(visible | alive),
	dirty_(false)
{}


void Node::add_node(const boost::shared_ptr<Node>& node)
{
	children_.push_back(node);
	node->parent_ = this;
	if (!dirty() && !dirty_)
	{
		node->apply_transform(transform());
		node->update_children_transform();
	}
}

void Node::draw_impl(Context& context)
{
	context.driver().draw(this);
}

void Node::update(cmn::uint tick)
{
	if (!is_frozen())
		update_impl(tick);
	if (dirty())
	{
		update_transform();
		clear_dirty_flag();
	}
	for (size_t i = 0; i < children_.size(); ++i)
		children_[i]->update(tick);
}

void Node::start()
{
	if (!is_frozen())
		start_impl(utils::get_current_tick());
}	

void Node::update_transform()
{
	Transform::update_transform();
	if (parent_ != nullptr)
		apply_transform(parent_->transform());
	update_children_transform();
}

void Node::update_children_transform()
{
	for (size_t i = 0; i < children_.size(); ++i)
	{
		children_[i]->Transform::update_transform();
		children_[i]->apply_transform(transform());
		children_[i]->update_children_transform();
		children_[i]->clear_dirty_flag();
	}
}

}
