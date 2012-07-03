#ifndef __ASPECT_MANAGER_HPP__
#define __ASPECT_MANAGER_HPP__

#include "aspect.hpp"

namespace mhe {
namespace game {

class AspectManager
{
public:
	void add(const aspect_ptr& aspect);
	aspect_ptr get(const std::string& full_name) const;

	void update(cmn::uint tick);
private:
	bool check_for_aspect_delete(const aspect_ptr& aspect) const;
	void destroy_aspect(const aspect_ptr& aspect);

	typedef std::map<std::string, aspect_ptr> aspect_map;
	aspect_map aspects_;
};

}}

#endif
