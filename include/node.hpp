#ifndef _INODE_HPP_
#define _INODE_HPP_

#include <boost/shared_ptr.hpp>
#include "math/matrix.hpp"
#include "context.hpp"
#include "texture.hpp"
#include "types.hpp"
#include "transform.hpp"
#include "utils/sysutils.hpp"

namespace mhe
{
// Base renderable class
class Node : public Transform
{	
public:
	enum
	{
		priority_low = 0,
		priority_normal = 5,
		priority_high = 10
	};
public:
	Node() : priority_(priority_normal)
	{
		texcoord_[0] = 0.0; texcoord_[1] = 0.0; texcoord_[2] = 0.0; texcoord_[3] = 1.0;
		texcoord_[4] = 1.0; texcoord_[5] = 1.0; texcoord_[6] = 1.0; texcoord_[7] = 0.0;
	}

	virtual ~Node() {}

	void draw(const Context& context)												
	{
		for (size_t i = 0; i < childs_.size(); ++i)
			childs_[i]->draw(context);
		draw_impl(context);
	}

	void update(cmn::uint tick)
	{
		update_impl(tick);
	}

	bool alive() const
	{
		return is_alive();
	}

	void set_priority(int pri)
	{
		priority_ = pri;
	}

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

	void start()
	{
		start_impl(utils::get_current_tick());
	}	

	Node* clone() const
	{
		return clone_impl();
	}

	void set_texture(const boost::shared_ptr<Texture>& texture)
	{
		texture_ = texture;
	}

	const boost::shared_ptr<Texture> texture() const
	{
		return texture_;
	}

	void set_texcoord(const Texture::texcoord& coord)
	{
		texcoord_ = coord;
	}

	void set_color(const colorf& color)
	{
		color_ = color;
	}

	const colorf& color() const
	{
		return color_;
	}
protected:
	Texture::texcoord texcoord() const
	{
		return texcoord_;
	}
private:
	virtual void draw_impl(const Context&) = 0;
	virtual void update_impl(cmn::uint) {}

	virtual bool is_alive() const {return true;}
	
	virtual void start_impl(cmn::uint) {}	
private:
	virtual Node* clone_impl() const
	{
		return nullptr;
	}		
	
	int priority_;						
	std::string name_;
	std::vector< boost::shared_ptr<Node> > childs_;	
	boost::shared_ptr<Texture> texture_;
	Texture::texcoord texcoord_;
	colorf color_;
};
	
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
};

#endif
