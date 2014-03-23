#ifndef __COMPONENT_HPP__
#define __COMPONENT_HPP__

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "message.hpp"
#include "types.hpp"
#include "ref_counter.hpp"
#include "math/matrix.hpp"

namespace mhe {
namespace game {

class Component;
typedef boost::shared_ptr<Component> component_ptr;
typedef boost::weak_ptr<Component> component_weak_ptr;

class Engine;

#define COMPONENT_METHODS(class_name, n)					 \
	friend class ComponentManager;                           \
	public:                                                  \
	class_name() {}                                          \
	static std::string type_name()                           \
	{ return n; }                                         \
    private:                                                 \
    std::string add_name_impl() const                        \
    { return n; }											                                    

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

	std::string add_name() const
	{
		return add_name_impl();
	}

	std::string full_name() const
	{
		return name_ + "." + add_name();
	}

	void attach(component_ptr component);
	void attach(component_ptr component, int type);
	void attach(component_ptr component, const std::vector<int>& types);

	void attach(Component* component)
	{
		attach(component_ptr(component));
	}

	void detach(Component* component);
	void detach(component_ptr component);
	void update(const Message& message);
	void update(cmn::uint tick, Engine* engine)
	{
		if (lifetime_ && !start_time_) start_time_ = tick;
		update_impl(tick, engine);
		for (size_t i = 0; i < children_.size(); ++i)
			children_[i]->update(tick, engine);
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

	component_ptr child_by_add_name(const std::string& add_name) const;

	// IMPORTANT: I think this function if VERY slow
	template <class T>
	T* get_component() const
	{
		for (size_t i = 0; i < children_.size(); ++i)
		{
			if (children_[i]->add_name() == T::type_name())
				return children_[i].get();
		}
		return nullptr;
	}

	template <class T>
	bool remove_component()
	{
		T* component = get_component<T>();
		if (component == nullptr) return;
		detach(component);
		return true;
	}

	mat4x4 transform() const
	{
		return transform_impl();
	}

	static std::string type_name()
	{
		return "";
	}
protected:
	Component() :
        parent_(nullptr), root_(nullptr),
		lifetime_(lifetime_infinite), start_time_(0)
	{}

	Component(const std::string& name) :
        name_(name), parent_(nullptr), root_(nullptr),
		lifetime_(lifetime_infinite), start_time_(0)
	{}

	void set_parent(Component* component)
	{
		parent_ = component;
	}

	Component* parent() const
	{
		return parent_;
	}

	void detach_self()
	{
		parent_->detach(this);
	}

	void send_message(const Message& message);

	virtual void do_subscribe(Component* component) = 0;
	void update_children(const Message& message); 
	virtual void update_impl(cmn::uint, Engine*)	{}
	virtual	bool update_impl(const Message& message) = 0;
	virtual void destroy_impl() {}

	void set_component_name(const std::string& name)
	{
		name_ = name;
	}
private:
	virtual void on_attach(Component* /*component*/) {}
	virtual void on_detach(Component* /*component*/) {}
	virtual std::string add_name_impl() const = 0;

	virtual mat4x4 transform_impl() const
	{
		return mat4x4::identity();
	}

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
