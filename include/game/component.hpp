#ifndef __COMPONENT_HPP__
#define __COMPONENT_HPP__

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "message.hpp"
#include "types.hpp"

namespace mhe {
namespace game {

class Component;
typedef boost::shared_ptr<Component> component_ptr;

class Component
{
public:
	static const int invalid_type = -1;
	static const cmn::uint lifetime_infinite = 0;
public:
	virtual ~Component() {}

	const std::string& name() const
	{
		return name_;
	}

	const std::string& add_name() const
	{
		return add_name_;
	}

	std::string full_name() const
	{
		return name_ + "." + add_name_;
	}

	void attach(component_ptr component);
	void attach(component_ptr component, int type);
	void attach(component_ptr component, const std::vector<int>& types);
	void detach(Component* component);
	void update(const Message& message);
	void update(cmn::uint tick)
	{
		if (lifetime_ && !start_time_) start_time_ = tick;
		update_impl(tick);
		for (size_t i = 0; i < children_.size(); ++i)
			children_[i]->update(tick);
	}

	void subscribe(int type, Component* component);

	void set_lifetime(cmn::uint lifetime)
	{
		lifetime_ = lifetime;
	}

	cmn::uint lifetime() const
	{
		return lifetime_;
	}

	cmn::uint start_time() const
	{
		return start_time_;
	}

	const std::vector<component_ptr>& children() const
	{
		return children_;
	}	
protected:
	Component(const std::string& fullname) :
		parent_(nullptr), root_(nullptr),
		lifetime_(lifetime_infinite),
		start_time_(0)
	{
		divide_full_name(fullname);
	}

	Component(const std::string& name, const std::string& add_name) :
		name_(name), add_name_(add_name), parent_(nullptr),
		lifetime_(lifetime_infinite), start_time_(0)
	{}

	void set_parent(Component* component)
	{
		parent_ = component;
	}

	void detach_self()
	{
		parent_->detach(this);
	}

	void send_message(const Message& message);

	virtual void do_subscribe(Component* component) = 0;
	void update_children(const Message& message); 
	virtual void update_impl(cmn::uint)	{}
	virtual	bool update_impl(const Message& message) = 0;
	virtual void destroy_impl() {}
private:
	void divide_full_name(const std::string& fullname)
	{
		size_t point_pos = fullname.find(".");
		if ( (point_pos != std::string::npos) && 
			 (point_pos < (fullname.length() - 1)) )
		{
			name_ = fullname.substr(0, point_pos);
			add_name_ = fullname.substr(point_pos + 1, fullname.length() - point_pos);
		}
		else name_ = fullname;
	}

	std::string name_;
	std::string add_name_;
	Component* parent_;
	Component* root_;
	std::vector<component_ptr> children_;
	typedef std::map< int, std::vector<Component*> > subsmap;
	subsmap subscribers_;
	cmn::uint lifetime_;
	cmn::uint start_time_;
};

}}	// namespaces

#endif