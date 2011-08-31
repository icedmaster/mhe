#ifndef __RESOURCE_MANAGER_HPP__
#define __RESOURCE_MANAGER_HPP__

#include <map>
#include <string>
#include "utils/file_utils.hpp"
#include "utils/logutils.hpp"

namespace mhe {

template <class Loader>
class ResourceManager
{
private:
	typedef typename Loader::type res_type;
	typedef std::map< std::string, boost::shared_ptr<res_type> > resmap;
	mutable resmap resources_;

	boost::shared_ptr<res_type> load_impl(const std::string& name,
								   const std::string& sname) const
	{
		res_type* res = Loader::load(name);
		if (res == 0)
		{
			utils::global_log::instance().printf("Can't load: %s", name.c_str());
			return boost::shared_ptr<res_type>();
		}
		utils::global_log::instance().printf("Resource %s loaded", name.c_str());
		boost::shared_ptr<res_type> shared_res(res);
		resources_[sname] = shared_res;
		return shared_res;
	}
public:
	bool load(const std::string& name)
	{
		return (get(name) != boost::shared_ptr<res_type>());
	}

	bool unload(const std::string& name)
	{
		typename resmap::iterator it = resources_.find(get_file_name(name));
		if (it == resources_.end()) return false;
		resources_.erase(it);
		return true;
	}

	bool have(const std::string& name) const
	{
		typename resmap::const_iterator it = resources_.find(name);
		if (it == resources_.end()) return false;
		return true;
	}

	boost::shared_ptr<res_type> get(const std::string& name) const
	{
		const std::string& sname = get_file_name(name);
		typename resmap::iterator it = resources_.find(sname);
		if (it != resources_.end())
			return it->second;
		return load_impl(name, sname);
	}
};

}	// namespace mhe

#endif
