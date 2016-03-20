#ifndef __RENDER_COMMON_HPP__
#define __RENDER_COMMON_HPP__

#include "core/types.hpp"
#include "core/pool.hpp"

namespace mhe {

typedef uint8_t MaterialSystemId;

typedef uint16_t ShaderProgramHandleType;
typedef uint16_t TextureHandleType;
typedef uint16_t LayoutHandleType;
typedef uint16_t RenderStateHandleType;
typedef uint16_t UniformBufferHandleType;
typedef uint16_t VertexBufferHandleType;
typedef uint16_t IndexBufferHandleType;
typedef uint16_t TextureBufferHandleType;
typedef uint16_t ShaderStorageBufferHandleType;
typedef uint16_t RenderTargetHandleType;
typedef uint16_t MaterialHandleType;

typedef uint16_t AABBInstanceHandleType;
typedef uint16_t MeshTraceDataHandleType;
typedef uint16_t MeshRawDataHandleType;

enum Primitive
{
    triangle = 0,
    line = 1,
    point = 2,
    gpu_generated = 3,
    primitive_max
};

}

#endif
