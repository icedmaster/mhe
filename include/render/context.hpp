#ifndef __CONTEXT_HPP__
#define __CONTEXT_HPP__

#include "window_system.hpp"
#include "video_driver.hpp"
#include "render_buffer.hpp"
#include "core/pool.hpp"
#include "res/resource_managers.hpp"

namespace mhe {

typedef Pool<VertexBuffer, uint16_t, 4096> VertexBufferPool;
typedef Pool<IndexBuffer, uint16_t, 4096> IndexBufferPool;
typedef Pool<Layout, uint16_t, 128> LayoutPool;

struct Context
{
	WindowSystem window_system;
	Driver driver;
	MeshResourceManager mesh_resource_manager;

	VertexBufferPool vertex_buffer_pool;
	IndexBufferPool index_buffer_pool;
	LayoutPool layout_pool;
};

}

#endif
