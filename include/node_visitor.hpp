#ifndef __NODE_VISITOR_HPP__
#define __NODE_VISITOR_HPP__

#include <string>

namespace mhe {

class Node;
class Sprite;

class NodeVisitor
{
public:
	NodeVisitor(const std::string& name) :
		name_(name)
	{}

	virtual ~NodeVisitor() {}

	const std::string& name() const
	{
		return name_;
	}

	virtual void begin_traverse() {}
	virtual void visit(Node* node) = 0;
	virtual void visit(Sprite* sprite) = 0;
	virtual void end_traverse() {}
	virtual std::list<Node*> nodes() const
	{
		return std::list<Node*>();
	}
private:
	std::string name_;
};

}

#endif
