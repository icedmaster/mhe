#ifndef __CONTEXT_HPP__
#define __CONTEXT_HPP__

#include "window_system.hpp"
#include "video_driver.hpp"
#include "render_buffer.hpp"
#include "core/pool.hpp"
#include "res/resource_managers.hpp"

namespace mhe {

typedef Pool<VertexBuffer, 4096, uint16_t> VertexBufferPool;
typedef Pool<IndexBuffer, 4096, uint16_t> IndexBufferPool;
typedef Pool<Layout, 128, uint16_t> LayoutPool;
typedef Pool<ShaderProgram, 128, uint16_t> ShaderPool;
typedef Pool<Transform, 4096, uint16_t> TransformPool;

struct Context
{
	WindowSystem window_system;
	Driver driver;

	MeshManager mesh_manager;
    ShaderManager shader_manager;

	VertexBufferPool vertex_buffer_pool;
	IndexBufferPool index_buffer_pool;
	LayoutPool layout_pool;
	ShaderPool shader_pool;

	TransformPool transform_pool;
};

}

#endif
