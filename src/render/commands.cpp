#include "render/commands.hpp"

#include "render/video_driver.hpp"

namespace mhe {

bool ClearCommand::execute()
{
    if (executed_) return true;
	ASSERT(driver_ != nullptr, "You must setup the driver first");
	driver_->clear(true, true, true, vec4(0.0, 0.0, 0.0, 0.0));
    executed_ = true;
	return true;
}

void ClearCommand::reset()
{
    executed_ = false;
}

bool ClearCommandSimple::execute()
{
	ASSERT(driver_ != nullptr, "You must setup the driver first");
	driver_->clear(true, true, true, vec4(0.0, 0.0, 0.0, 0.0));
	return true;
}

bool SetRenderTargetTextureSideCommand::execute()
{
	if (executed_) return true;
	executed_ = true;
	render_target_->set_texture_side(side_);
	driver_->clear(true, true, true);
	return true;
}

}