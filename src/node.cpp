#include "node.hpp"

namespace mhe {

Node::Node() : 
	Renderable(),
	priority_(priority_normal), flags_(visible | alive)
{
}

void Node::draw_impl(const Context& context)
{
}

}
