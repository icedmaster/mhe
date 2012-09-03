#ifndef __MESSAGE_TRANSFORM_HPP__
#define __MESSAGE_TRANSFORM_HPP__

#include "message.hpp"
#include "transform.hpp"

namespace mhe {
namespace game {

class TransformMessage : public Message, public Transform
{
private:
	int get_type_impl() const
	{
		return transform_event;
	}
};

}}

#endif
