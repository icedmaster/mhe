#ifndef __MESSAGE_EMPTY_HPP__
#define __MESSAGE_EMPTY_HPP__

#include "message.hpp"
#include "aspect_event_types.hpp"

namespace mhe {
namespace game {

class EmptyMessage : public Message
{
private:
	int get_type_impl() const
	{
		return empty_event;
	}
};

}}

#endif
