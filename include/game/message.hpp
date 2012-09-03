#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#include "utils/properties_list.hpp"
#include "message_event_types.hpp"

namespace mhe {
namespace game {

class Message
{
public:
	virtual ~Message() {}

	int type() const
	{
		return get_type_impl();
	}
	
	template <class T>
	T get_property(const std::string& key)
	{
		return properties_.get<T>(key);
	}
protected:
	template <class T>
	void set_property(const std::string& key, const T& value)
	{
		properties_.add(key, value);
	}
private:
	virtual int get_type_impl() const = 0;
private:
	utils::PropertiesList properties_;
};

/// Helper class for creating simple messages
template <int Type>
class SimpleMessage : public Message
{
private:
	int get_type_impl() const
	{
		return Type;
	}
};

}}

#endif
