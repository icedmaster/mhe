#include "render/render_state.hpp"

#include "impl/system_factory.hpp"

namespace mhe {

RenderState::RenderState() :
    impl_(SystemFactory::instance().create_render_state())
{}

RenderState::~RenderState()
{
    SystemFactory::instance().destroy_object(impl_);
}

}
