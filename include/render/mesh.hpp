#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <vector>

namespace mhe {

struct Mesh
{
	uint32_t transform_id;
	uint32_t vbuffer_id;
	uint32_t ibuffer_id;
	uint16_t material_id;
};

}

#endif
