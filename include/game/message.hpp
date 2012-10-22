#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#include "utils/properties_list.hpp"
#include "message_event_types.hpp"
#include "types.hpp"

namespace mhe {
namespace game {

class Component;

class Message
{
public:	
	Message(Component* sender = nullptr):
	sender_(sender)
	{}

	virtual ~Message() {}

	int type() const
	{
		return get_type_impl();
	}

	const Component* sender() const
	{
		return sender_;
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
	Component* sender_;
};

/// Helper class for creating simple messages
template <int Type>
class SimpleMessage : public Message
{
public:
	SimpleMessage(Component* sender = nullptr) :
	Message(sender)
	{}
private:
	int get_type_impl() const
	{
		return Type;
	}
};

}}

#endif
