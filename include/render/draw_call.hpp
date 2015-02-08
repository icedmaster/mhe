#ifndef __DRAW_CALL_HPP__
#define __DRAW_CALL_HPP__

#include "node.hpp"

namespace mhe {

struct DrawCall
{
    DrawCallData::IdType draw_call_data;
    MaterialInstance material;
    RenderData render_data;
    RenderCommand* command;

    DrawCall() : draw_call_data(DrawCallData::invalid_id), command(nullptr) {}
};

}

#endif // DRAW_CALL_HPP
