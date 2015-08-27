#include "render/layouts.hpp"

#include "render/context.hpp"

namespace mhe {

void StandartGeometryLayout::init(Context& context)
{
	context.layout_pool.create();
	
	LayoutDesc desc;
	desc.layout.resize(4);
	desc.layout[0].offset = 0;
	desc.layout[0].size = 3;
	desc.layout[0].stride = sizeof(Vertex);
	desc.layout[0].position = 0;
	desc.layout[0].datatype = format_float;

	desc.layout[1].offset = 3 * sizeof(float);
	desc.layout[1].size = 3;
	desc.layout[1].stride = sizeof(Vertex);
	desc.layout[1].position = 1;
	desc.layout[1].datatype = format_float;

	desc.layout[2].offset = 6 * sizeof(float);
	desc.layout[2].size = 4;
	desc.layout[2].stride = sizeof(Vertex);
	desc.layout[2].position = 2;
	desc.layout[2].datatype = format_float;

	desc.layout[3].offset = 10 * sizeof(float);
	desc.layout[3].size = 2;
	desc.layout[3].stride = sizeof(Vertex);
	desc.layout[3].position = 3;
	desc.layout[3].datatype = format_float;

	context.layout_pool.get(handle).init(desc);
}

void SkyboxLayout::init(Context& context)
{
	context.layout_pool.create();
	
	LayoutDesc desc;
	desc.layout.resize(1);
	desc.layout[0].offset = 0;
	desc.layout[0].size = 4;
	desc.layout[0].stride = sizeof(Vertex);
	desc.layout[0].position = 0;
	desc.layout[0].datatype = format_float;

	context.layout_pool.get(handle).init(desc);
}

void FullscreenLayout::init(Context& context)
{
	context.layout_pool.create();
	
	LayoutDesc desc;
	desc.layout.resize(2);
	desc.layout[0].offset = 0;
	desc.layout[0].size = 4;
	desc.layout[0].stride = sizeof(Vertex);
	desc.layout[0].position = 0;
	desc.layout[0].datatype = format_float;
	desc.layout[1].offset = 4 * sizeof(float);
	desc.layout[1].size = 2;
	desc.layout[1].stride = sizeof(Vertex);
	desc.layout[1].position = 1;
	desc.layout[1].datatype = format_float;

	context.layout_pool.get(handle).init(desc);
}

void DebugLayout::init(Context& context)
{
	context.layout_pool.create();

    LayoutDesc desc;
    desc.layout.resize(1);
    desc.layout[0].offset = 0;
    desc.layout[0].size = 3;
    desc.layout[0].stride = sizeof(Vertex);
    desc.layout[0].position = 0;
	desc.layout[0].datatype = format_float;

    context.layout_pool.get(handle).init(desc);
}


void SkinnedGeometryLayout::init(Context& context)
{
	context.layout_pool.create();

	LayoutDesc desc;
	desc.layout.resize(6);
	desc.layout[0].offset = 0;
	desc.layout[0].size = 3;
	desc.layout[0].stride = sizeof(Vertex);
	desc.layout[0].position = 0;
	desc.layout[0].datatype = format_float;

	desc.layout[1].offset = 3 * sizeof(float);
	desc.layout[1].size = 3;
	desc.layout[1].stride = sizeof(Vertex);
	desc.layout[1].position = 1;
	desc.layout[1].datatype = format_float;

	desc.layout[2].offset = 6 * sizeof(float);
	desc.layout[2].size = 4;
	desc.layout[2].stride = sizeof(Vertex);
	desc.layout[2].position = 2;
	desc.layout[2].datatype = format_float;

	desc.layout[3].offset = 10 * sizeof(float);
	desc.layout[3].size = 2;
	desc.layout[3].stride = sizeof(Vertex);
	desc.layout[3].position = 3;
	desc.layout[3].datatype = format_float;

	desc.layout[4].offset = 12 * sizeof(float);
	desc.layout[4].size = 4;
	desc.layout[4].stride = sizeof(Vertex);
	desc.layout[4].position = 4;
	desc.layout[4].datatype = format_int;

	desc.layout[5].offset = 12 * sizeof(float) + 4 * sizeof(uint);
	desc.layout[5].size = 4;
	desc.layout[5].stride = sizeof(Vertex);
	desc.layout[5].position = 5;
	desc.layout[5].datatype = format_float;

	context.layout_pool.get(handle).init(desc);
}

bool init_standart_layouts(Context& context)
{
	StandartGeometryLayout::init(context);
	SkyboxLayout::init(context);
	FullscreenLayout::init(context);
	DebugLayout::init(context);
	SkinnedGeometryLayout::init(context);

	return true;
}

}
