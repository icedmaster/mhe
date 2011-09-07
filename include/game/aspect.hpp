#ifndef __ASPECT_HPP__
#define __ASPECT_HPP__

#include <string>
#include <vector>
#include "types.hpp"

namespace mhe {
namespace game {

struct AspectParam
{
	virtual ~AspectParam() {}
};

class Aspect
{
public:
enum
{
	DrawEvent,
	TickEvent
};
public:
	virtual ~Aspect() {}

	const std::string& get_name() const
	{
		return name_;
	}

	void add(Aspect* aspect);

	void update(int aspect_event, cmn::uint tick, const AspectParam& prm)
	{
		update_impl(aspect_event, tick, prm);
	}

protected:
	Aspect() {}

	void set_parent(Aspect* aspect)
	{
		parent_ = aspect;
	}

	virtual void update_impl(int aspect_event, cmn::uint tick, const AspectParam& prm);
private:
	std::string name_;
	Aspect* parent_;
	std::vector<Aspect*> childs_;
};

}}	// namespaces

#endif
