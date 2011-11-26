#ifndef __ASPECT_HPP__
#define __ASPECT_HPP__

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "aspect_event_types.hpp"
#include "types.hpp"

namespace mhe {
namespace game {

class Aspect;
typedef boost::shared_ptr<Aspect> aspect_ptr;

class Aspect
{
public:
	static const int invalid_type = -1;
public:
	virtual ~Aspect() {}

	const std::string& name() const
	{
		return name_;
	}

	void attach(aspect_ptr aspect);
	void attach(aspect_ptr aspect, int type);
	void attach(aspect_ptr aspect, const std::vector<int>& types);
	void update(int type, const void* prm);

	void subscribe(int type, Aspect* aspect);
protected:
	Aspect(const std::string& name) : name_(name) {}

	void set_parent(Aspect* aspect)
	{
		parent_ = aspect;
	}

	virtual void do_subscribe(Aspect* aspect) = 0;
	void update_childs(int type, const void* prm); 
	virtual	bool update_impl(int type, const void* prm) = 0;
private:
	typedef boost::weak_ptr<Aspect> aspect_weak_ptr;
	std::string name_;
	Aspect* parent_;
	std::vector<aspect_weak_ptr> childs_;
	typedef std::map< int, std::vector<Aspect*> > subsmap;
	subsmap subscribers_;
};

}}	// namespaces

#endif
