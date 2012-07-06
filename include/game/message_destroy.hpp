#ifndef __MESSAGE_DESTROY_HPP__
#define __MESSAGE_DESTROY_HPP__

#include "message.hpp"
#include "aspect_event_types.hpp"

namespace mhe {
namespace game {

class DestroyMessage : public Message
{
private:
	int get_type_impl() const
	{
		return destroy_event;
	}
};

}}

#endif
