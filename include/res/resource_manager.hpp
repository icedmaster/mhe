#ifndef __RESOURCE_MANAGER_HPP__
#define __RESOURCE_MANAGER_HPP__

#include "core/hashmap.hpp"
#include "core/ref_counter.hpp"
#include "core/string.hpp"
#include "utils/file_utils.hpp"
#include "utils/logutils.hpp"

namespace mhe {

template <class Loader>
class ResourceManager
{
public:
    struct LoadingParameters
    {
        uint32_t flags;

        LoadingParameters() : flags(0) {}
    };
protected:
    typedef typename Loader::type res_type;
    typedef typename Loader::instance_type instance_type;
    typedef typename Loader::context_type context_type;

    struct Info
    {
        ref_counter refs;
        res_type res;
    };

    typedef hashmap< FilePath, Info > resmap;
public:
    virtual ~ResourceManager() {}

    void set_context(context_type* context)
    {
        context_ = context;
    }

    bool load(const string& name, bool absolute_path = false)
    {
        return get(name, absolute_path);
    }

    bool unload(const string& name)
    {
        typename resmap::iterator it = resources_.find(utils::get_file_name(name));
        if (it == resources_.end()) return false;
        resources_.erase(it);
        return true;
    }

    bool get(res_type& res, const FilePath& name, bool absolute_path = false) const
    {
        return get_impl(res, name, absolute_path);
    }

    bool get(res_type& res, const string& name, bool absolute_path = false) const
    {
        return get_impl(res, FilePath(name), absolute_path);
    }

    bool get_instance(instance_type& instance, const string& name, bool absolute_path = false) const
    {
        res_type& res = Loader::get_resource(instance);
        if (!get(res, name, absolute_path))
            return false;
        return Loader::setup_instance(instance, context_);
    }

    bool has(const FilePath& name) const
    {
        return resources_.find(name) != resources_.end();
    }

    void clear()
    {
        resources_.clear();
    }

    void set_path(const FilePath& path)
    {
        path_ = path;
    }

    const FilePath& path() const
    {
        return path_;
    }

    void add(const res_type& res, const FilePath& name)
    {
        Info& info = resources_[name];
        info.res = res;
    }
protected:
    virtual bool get_impl(res_type& res, const FilePath& name, bool absolute_path) const
    {
        typename resmap::iterator it = resources_.find(name);
        if (it != resources_.end())
        {
            it->value.refs.add_ref();
            res = it->value.res;
            return true;
        }
        return load_impl(res, name, absolute_path);
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
    bool load_impl(res_type& res, const FilePath& name,
                   bool absolute_path) const
    {
        FilePath full_path = (!absolute_path) ? utils::path_join(path_, name) : FilePath(name);
        bool result = Loader::load(res, full_path.c_str(), context_);
        if (!result)
        {
            ERROR_LOG("Can't load: " << full_path);
            return false;
        }
        INFO_LOG("Resource " << name << " loaded");
        Info info;
        info.res = res;
        info.refs.add_ref();
        resources_[name] = info;
        return true;
    }

    mutable resmap resources_;
    context_type* context_;
    FilePath path_;
};

}   // namespace mhe

#endif
