#ifndef __KEYVALUE_HPP__
#define __KEYVALUE_HPP__

#include "hashmap.hpp"
#include "types_cast.hpp"

namespace mhe {

template <class K, class S>
class KeyValue
{
    typedef hashmap<K, S> StorageMap;
public:
    template <class V>
    void add(const K& key, const V& value)
    {
        storage_[key] = types_cast<S>(value);
    }

    void add(const K& key, const S& value)
    {
        storage_[key] = value;
    }

    template <class V>
    V get(const K& key) const
    {
        typename StorageMap::const_iterator it = storage_.find(key);
        if (it == storage_.end()) return V();
        return types_cast<V>(it->value);
    }

    bool has(const K& key) const
    {
        return storage_.find(key) != storage_.end();
    }

    void clear()
    {
        storage_.clear();
    }
private:
    StorageMap storage_;
};

}

#endif
