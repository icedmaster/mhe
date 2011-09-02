#ifndef __GRAPHICS_ASPECT_HPP__
#define __GRAPHICS_ASPECT_HPP__

#include "aspect.hpp"
#include "inode.hpp"

namespace mhe {
namespace game {

struct GraphicsAspectParam : public AspectParam
{
	boost::shared_ptr<iDriver> driver;
};

class GraphicsAspect : public Aspect
{
public:
	GraphicsAspect(boost::shared_ptr<iNode> node) :
		node_(node)
	{}

protected:
	void update_impl(int aspect_event, cmn::uint tick, const AspectParam& prm);
private:
	boost::shared_ptr<iNode> node_;
};

}}

#endif
