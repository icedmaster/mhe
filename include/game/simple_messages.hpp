#ifndef __SIMPLE_MESSAGES_HPP__
#define __SIMPLE_MESSAGES_HPP__

#include "message.hpp"

namespace mhe {
namespace game {

typedef SimpleMessage<empty_event> EmptyMessage;
typedef SimpleMessage<destroy_event> DestroyMessage;
typedef SimpleMessage<end_event> EndEventMessage;

}}

#endif
