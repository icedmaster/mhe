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
	desc.layout[0].size = 3;
	desc.layout[0].stride = sizeof(Vertex);
	desc.layout[0].position = 0;

	desc.layout[1].offset = 3 * sizeof(float);
	desc.layout[1].size = 3;
	desc.layout[1].stride = sizeof(Vertex);
	desc.layout[1].position = 1;

	desc.layout[2].offset = 6 * sizeof(float);
	desc.layout[2].size = 3;
	desc.layout[2].stride = sizeof(Vertex);
	desc.layout[2].position = 2;

	desc.layout[3].offset = 9 * sizeof(float);
	desc.layout[3].size = 2;
	desc.layout[3].stride = sizeof(Vertex);
	desc.layout[3].position = 3;

	context.layout_pool.get(handle).init(desc);
}

uint16_t SkyboxLayout::handle = 1;

void SkyboxLayout::init(Context& context)
{
	handle = context.layout_pool.create();
	
	LayoutDesc desc;
	desc.layout.resize(1);
	desc.layout[0].offset = 0;
	desc.layout[0].size = 4;
	desc.layout[0].stride = sizeof(Vertex);
	desc.layout[0].position = 0;

	context.layout_pool.get(handle).init(desc);
}

uint16_t FullscreenLayout::handle = 2;

void FullscreenLayout::init(Context& context)
{
	handle = context.layout_pool.create();
	
	LayoutDesc desc;
	desc.layout.resize(2);
	desc.layout[0].offset = 0;
	desc.layout[0].size = 4;
	desc.layout[0].stride = sizeof(Vertex);
	desc.layout[0].position = 0;
	desc.layout[1].offset = 4 * sizeof(float);
	desc.layout[1].size = 2;
	desc.layout[1].stride = sizeof(Vertex);
	desc.layout[1].position = 1;

	context.layout_pool.get(handle).init(desc);
}

uint16_t DebugLayout::handle = 3;

void DebugLayout::init(Context& context)
{
    handle = context.layout_pool.create();
    LayoutDesc desc;
    desc.layout.resize(1);
    desc.layout[0].offset = 0;
    desc.layout[0].size = 3;
    desc.layout[0].stride = sizeof(Vertex);
    desc.layout[0].position = 0;

    context.layout_pool.get(handle).init(desc);
}

bool init_standart_layouts(Context& context)
{
	StandartGeometryLayout::init(context);
	SkyboxLayout::init(context);
	FullscreenLayout::init(context);
	DebugLayout::init(context);

	return true;
}

}
