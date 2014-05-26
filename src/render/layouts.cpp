#include "render/layouts.hpp"

#include "render/context.hpp"

namespace mhe {

uint16_t StandartGeometryLayout::handle = 0;

void StandartGeometryLayout::init(Context& context)
{
	handle = context.layout_pool.create();
	
	LayoutDesc desc;
	desc.layout.resize(4);
	desc.layout[0].offset = 0;
	desc.layout[0].size = 3 * sizeof(float);
	desc.layout[0].stride = sizeof(Vertex);
	desc.layout[0].position = 0;

	desc.layout[1].offset = 3 * sizeof(float);
	desc.layout[1].size = 3 * sizeof(float);
	desc.layout[1].stride = sizeof(Vertex);
	desc.layout[1].position = 1;

	desc.layout[2].offset = 6 * sizeof(float);
	desc.layout[2].size = 3 * sizeof(float);
	desc.layout[2].stride = sizeof(Vertex);
	desc.layout[2].position = 2;

	desc.layout[3].offset = 9 * sizeof(float);
	desc.layout[3].size = 2 * sizeof(float);
	desc.layout[3].stride = sizeof(Vertex);
	desc.layout[3].position = 3;

	context.layout_pool.get(handle).init(desc);
}

}
