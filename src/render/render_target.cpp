#include "render/render_target.hpp"

#include "impl/system_factory.hpp"

namespace mhe {

RenderTarget::RenderTarget() :
	impl_(SystemFactory::instance().create_render_target())
{}

}
