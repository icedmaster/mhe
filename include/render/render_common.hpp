#ifndef __RENDER_COMMON_HPP__
#define __RENDER_COMMON_HPP__

#include "core/types.hpp"

namespace mhe {

typedef uint8_t MaterialSystemId;

typedef uint16_t ShaderProgramHandleType;
typedef uint16_t TextureHandleType;
typedef uint16_t DrawCallDataHandleType;
typedef uint16_t LayoutHandleType;
typedef uint16_t RenderStateHandleType;
typedef uint16_t UniformBufferHandleType;
typedef uint16_t VertexBufferHandleType;
typedef uint16_t IndexBufferHandleType;
typedef uint16_t TextureBufferHandleType;

typedef uint16_t AABBInstanceHandleType;
typedef uint16_t MeshTraceDataHandleType;

template <class T>
struct InvalidHandle
{
	static const T id = static_cast<T>(-1);
};

enum Primitive
{
	triangle = 0,
	lines = 1
};

}

#endif
