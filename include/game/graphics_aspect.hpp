#ifndef __GRAPHICS_ASPECT_HPP__
#define __GRAPHICS_ASPECT_HPP__

#include "aspect.hpp"
#include "inode.hpp"
#include <boost/bind.hpp>

namespace mhe {
namespace game {

class GraphicsAspect : public Aspect
{
public:
	GraphicsAspect(const std::string name, boost::shared_ptr<iNode> node) :
		Aspect(name),
		node_(node)
	{
	}

	virtual ~GraphicsAspect() {}
private:
	virtual void do_subscribe(Aspect* parent);
	bool update_impl(int, const void*) {return true;}
	boost::shared_ptr<iNode> node_;
};

}}

#endif
