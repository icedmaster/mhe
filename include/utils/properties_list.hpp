#ifndef __PROPERTIES_LIST_HPP__
#define __PROPERTIES_LIST_HPP__

#include <string>
#include "types_cast.hpp"

namespace mhe {
namespace utils {

class PropertiesList
{
public:
	static PropertiesList empty()
	{
		return PropertiesList();
	}
public:
	template <class T>
	void add(const std::string& key, const T& value)
	{
		values_[key] = types_cast<std::string>(value);
	}

	template <class T>
	T get(const std::string& key) const
	{
		std::map<std::string, std::string>::const_iterator it = values_.find(key);
		if (it == values_.end()) return T();
		return types_cast<T>(it->second);
	}
private:
	std::map<std::string, std::string> values_;
};

}}

#endif
