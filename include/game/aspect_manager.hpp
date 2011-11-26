#ifndef __ASPECT_MANAGER_HPP__
#define __ASPECT_MANAGER_HPP__

#include "aspect.hpp"
#include <boost/shared_ptr.hpp>

namespace mhe {
namespace game {

class AspectManager
{
public:
	void add(Aspect* aspect);
	void add(boost::shared_ptr<Aspect> aspect);

	void update(cmn::uint tick);

	void remove(const std::string& name);
	void remove(Aspect* aspect);
	void remove(boost::shared_ptr<Aspect> aspect);
private:
	typedef boost::shared_ptr<Aspect> aspect_ptr;
	std::vector<aspect_ptr> aspects_;
};

}}

#endif
