#ifndef __RESOURCE_MANAGER_HPP__
#define __RESOURCE_MANAGER_HPP__

#include <map>
#include <string>
#include "core/ref_ptr.hpp"
#include "utils/file_utils.hpp"
#include "utils/logutils.hpp"

namespace mhe {

template <class Loader>
class ResourceManager
{
protected:
	typedef typename Loader::type res_type;
	typedef typename Loader::context_type context_type;
	typedef std::map< std::string, res_type > resmap;
public:
	virtual ~ResourceManager() {}

	void set_context(const context_type* context)
	{
		context_ = context;
	}

	bool load(const std::string& name, bool absolute_path = false)
	{
		return get(name, absolute_path);
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

	bool get(res_type& res, const std::string& name, bool absolute_path = false) const
	{
		return get_impl(res, name, absolute_path);	
	}

	void clear()
	{
		resources_.clear();
	}

	void set_path(const std::string& path)
	{
		path_ = path;
	}
protected:
	virtual bool get_impl(res_type& res, const std::string& name, bool absolute_path) const
	{
		const std::string& sname = utils::get_file_name(name);
		typename resmap::iterator it = resources_.find(sname);
		if (it != resources_.end())
		{
			INFO_LOG("get resource:" << sname);
			res = it->second;
			return true;
		}
		return load_impl(res, name, sname, absolute_path);
	}

    const context_type* context() const
	{
        return context_;
	}

	resmap& resources() const
	{
		return resources_;
	}
private:
	bool load_impl(res_type& res, const std::string& name,
								const std::string& sname,
								bool absolute_path) const
	{
		std::string full_path = (!absolute_path) ? utils::path_join(path_, name) : name;
        bool result = Loader::load(res, full_path, context_);
		if (!result)
		{
			ERROR_LOG("Can't load: " << full_path);
			return false;
		}
		INFO_LOG("Resource " << name << " loaded with sname:" << sname);
		resources_[sname] = res;
		return true;
	}

	mutable resmap resources_;
    const context_type* context_;
	std::string path_;
};

}	// namespace mhe

#endif
