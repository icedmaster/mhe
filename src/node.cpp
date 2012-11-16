#include "node.hpp"

#include "utils/sysutils.hpp"

namespace mhe {

Node::Node() : 
	Renderable(),
	priority_(priority_normal), flags_(visible | alive)
{
}

void Node::draw_impl(const Context& context)
{
	context.driver()->draw(this);
}

void Node::start()
{
	if (!is_frozen())
		start_impl(utils::get_current_tick());
}	

}
