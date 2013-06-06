#include "node.hpp"

#include "utils/sysutils.hpp"

namespace mhe {

Node::Node() : 
	Renderable(),
	priority_(priority_normal),	parent_(nullptr),
	flags_(visible | alive)
{
}

void Node::draw_impl(const Context& context)
{
	context.driver()->draw(this);
}

void Node::update(cmn::uint tick)
{
	if (!is_frozen())
		update_impl(tick);
	if (dirty())
	{
		update_children_transform();
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

void Node::update_children_transform()
{
	for (size_t i = 0; i < children_.size(); ++i)
	{
		children_[i]->apply_transform(transform());
		children_[i]->update_children_transform();
	}
}

}
