#ifndef __RENDER_COMMON_HPP__
#define __RENDER_COMMON_HPP__

#include "core/types.hpp"

namespace mhe {

typedef uint16_t AABBInstanceHandleType;
typedef uint16_t MeshTraceDataHandleType;

template <class T>
struct InvalidHandle
{
	static const T id = static_cast<T>(-1);
};

}

#endif
