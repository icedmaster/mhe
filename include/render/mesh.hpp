#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "render_buffer.hpp"

namespace mhe {

struct Mesh
{
	uint32_t vbuffer;
	uint32_t ibuffer;
	uint16_t layout;
};

}

#endif
