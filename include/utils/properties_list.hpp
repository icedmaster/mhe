#ifndef __PROPERTIES_LIST_HPP__
#define __PROPERTIES_LIST_HPP__

#include <string>
#include <boost/lexical_cast.hpp>

namespace mhe {
namespace utils {

class PropertiesList
{
public:
	template <class T>
	void add(const std::string& key, const T& value)
	{
		values_[key] = boost::lexical_cast<std::string>(value);
	}

	template <class T>
	T get(const std::string& key) const
	{
		std::map<std::string, std::string>::const_iterator it = values_.find(key);
		if (it == values_.end()) return T();
		return it->second;
	}
private:
	std::map<std::string, std::string> values_;
};

}}

#endif
