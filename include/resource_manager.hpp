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
	typedef typename Loader::type res_type;
	typedef typename Loader::helper_type helper_type;
	typedef std::map< std::string, boost::shared_ptr<res_type> > resmap;
public:
	void set_helper(const helper_type& helper)
	{
		helper_ = helper;
	}

	bool load(const std::string& name)
	{
		return (get(name) != boost::shared_ptr<res_type>());
	}

	bool unload(const std::string& name)
	{
		typename resmap::iterator it = resources_.find(utils::get_file_name(name));
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
		const std::string& sname = utils::get_file_name(name);
		typename resmap::iterator it = resources_.find(sname);
		if (it != resources_.end())
		{
			INFO_LOG("get resource:" << sname);
			return it->second;
		}
		return load_impl(utils::path_join(base_path_, name), sname);
	}

	void add(const std::string& name, res_type* res)
	{
		add(name, boost::shared_ptr<res_type>(res));
	}

	void add(const std::string& name, const boost::shared_ptr<res_type>& res)
	{
		resources_[name] = res;
	}

	void free_all()
	{
		resources_.clear();
	}
    
    void set_base_path(const std::string& path)
    {
        base_path_ = path;
    }
private:
	boost::shared_ptr<res_type> load_impl(const std::string& name,
										  const std::string& sname) const
	{
		res_type* res = Loader::load(name, helper_);
		if (res == nullptr)
		{
			ERROR_LOG("Can't load: " << name.c_str());
			return boost::shared_ptr<res_type>();
		}
		INFO_LOG("Resource " << name << " loaded with sname:" << sname);
		boost::shared_ptr<res_type> shared_res(res);
		resources_[sname] = shared_res;
		return shared_res;
	}

	mutable resmap resources_;
	helper_type helper_;
    std::string base_path_;
};

}	// namespace mhe

#endif
