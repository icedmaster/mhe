#include "render/commands.hpp"

#include "render/video_driver.hpp"

namespace mhe {

bool ClearCommand::execute_impl(RenderStage /*current_stage*/)
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

bool ClearCommandSimple::execute_impl(RenderStage /*current_stage*/)
{
	ASSERT(driver_ != nullptr, "You must setup the driver first");
	driver_->clear(true, true, true, vec4(0.0, 0.0, 0.0, 0.0));
	return true;
}

bool SetRenderTargetTextureSideCommand::execute_impl(RenderStage /*current_stage*/)
{
	if (executed_) return true;
	executed_ = true;
	render_target_->set_texture_side(side_);
	driver_->clear(true, true, true);
	return true;
}

CopyFramebufferCommand::CopyFramebufferCommand()
{
	set_stages(render_stage_before_render_target_setup);
}

bool CopyFramebufferCommand::execute_impl(RenderStage /*current_stage*/)
{
	texture_->copy_framebuffer();
	return true;
}

bool ListOfCommands::execute_impl(RenderStage current_stage)
{
	for (size_t i = 0, size = commands_.size(); i < size; ++i)
	{
		if (!(commands_[i]->stages() & current_stage)) continue;
		commands_[i]->execute(current_stage);
	}
	return true;
}
}
