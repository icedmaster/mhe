#include "game/graphics_aspect.hpp"

namespace mhe {
namespace game {

void GraphicsAspect::update_impl(int aspect_event, cmn::uint tick, const AspectParam& prm)
{
	switch (aspect_event)
	{
		case DrawEvent: 
			node_->draw(dynamic_cast<const GraphicsAspectParam&>(prm).driver);
			break;
		case TickEvent: node_->update(tick); break;
		default: break;
	}
	Aspect::update(aspect_event, tick, prm);
}

}}	// namespaces
