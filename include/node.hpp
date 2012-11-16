#ifndef __INODE_HPP__
#define __INODE_HPP__

#include <boost/shared_ptr.hpp>
#include "math/matrix.hpp"
#include "context.hpp"
#include "texture.hpp"
#include "types.hpp"
#include "renderable.hpp"
#include "node_visitor.hpp"

namespace mhe
{
// Base renderable class
class Node : public Renderable
{	
	friend class Scene;

	static const size_t default_texcoord_size = 8;
public:
	enum
	{
		priority_low = 0,
		priority_normal = 5,
		priority_high = 10
	};

	enum
	{
		visible = 1,
		frozen = (1 << 1),
		alive = (1 << 2)
	};
public:
	Node();
	virtual ~Node() {}

	void draw(const Context& context)												
	{
		for (size_t i = 0; i < childs_.size(); ++i)
			childs_[i]->draw(context);
		draw_impl(context);
	}

	void update(cmn::uint tick)
	{
		if (!is_frozen())
			update_impl(tick);
	}

	bool is_alive() const
	{
		return (flags_ & alive);
	}

	bool is_visible() const
	{
		return (flags_ & visible);
	}

	bool is_frozen() const
	{
		return (flags_ & frozen);
	}

	void start();

	int priority() const
	{
		return priority_;
	}				

	void set_name(const std::string& name)
	{
		name_ = name;
	}

	const std::string& name() const
	{
		return name_;
	}

	void add_node(boost::shared_ptr<Node> node)
	{
		childs_.push_back(node);
	}

	Node* clone() const
	{
		return clone_impl();
	}

	void apply(NodeVisitor* visitor)
	{
		apply_impl(visitor);
	}

	void set_priority(int pri)
	{
		priority_ = pri;
	}
protected:
	virtual void draw_impl(const Context&);
	virtual void apply_impl(NodeVisitor* visitor)
	{
		visitor->visit(this);
	}
private:
	virtual void update_impl(cmn::uint) {}	
	virtual void start_impl(cmn::uint) {}	
private:
	virtual Node* clone_impl() const
	{
		return nullptr;
	}		
	
	int priority_;						
	std::string name_;
	std::vector< boost::shared_ptr<Node> > childs_;	
	uint32_t flags_;
};

typedef boost::shared_ptr<Node> nodeptr;
	
// helper classes
class sort_node_by_pri
{
public:
	bool operator()(boost::shared_ptr<Node> node1,
					boost::shared_ptr<Node> node2)
	{
		return (node1->priority() < node2->priority());
	}
};

}

#endif
