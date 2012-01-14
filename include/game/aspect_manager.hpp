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

	void remove(const std::string& fullname);
	void remove(Aspect* aspect);
	void remove(boost::shared_ptr<Aspect> aspect);

	size_t get_by_name(const std::string& name,
					   std::vector< boost::weak_ptr<Aspect> >& aspects) const;
	size_t get_by_add_name(const std::string& name,
						   std::vector< boost::weak_ptr<Aspect> >& aspects) const;
	bool get_by_fullname(const std::string& fullname, boost::weak_ptr<Aspect>& aspect) const;
private:
	bool check_aspect_lifetime(cmn::uint tick, boost::shared_ptr<Aspect> aspect) const;
	void destroy_aspect(Aspect* aspect);
	typedef boost::shared_ptr<Aspect> aspect_ptr;
	typedef std::map<std::string, aspect_ptr> aspects_map;
	aspects_map aspects_;
};

}}

#endif
