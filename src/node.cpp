#include "node.hpp"

#include "utils/sysutils.hpp"

namespace mhe {

Node::Node() : 
	Renderable(),
	priority_(priority_normal),
	parent_(nullptr),
	flags_(visible | alive)
{
}

void Node::add_node(const boost::shared_ptr<Node>& node)
{
	children_.push_back(node);
	node->parent_ = this;
	node->apply_transform(transform());
	node->update_children_transform();
}

void Node::draw_impl(Context& context)
{
	context.driver().draw(this);
}

void Node::update(cmn::uint tick)
{
	if (!is_frozen())
		update_impl(tick);
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
	}
}

}
