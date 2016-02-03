#include "render/commands.hpp"

#include "render/video_driver.hpp"
#include "debug/gpu_profiler.hpp"

namespace mhe {

ClearCommand::ClearCommand() :
	executed_(false), clear_color_(true), clear_depth_(true), clear_stencil_(true)
{}

bool ClearCommand::execute_impl(RenderStage /*current_stage*/)
{
	if (executed_) return true;
	ASSERT(driver_ != nullptr, "You must setup the driver first");
    driver_->clear(clear_color_, clear_depth_, clear_stencil_, vec4(0.0, 0.0, 0.0, 1.0));
	executed_ = true;
	return true;
}

void ClearCommand::set_clear_mask(bool color, bool depth, bool stencil)
{
	clear_color_ = color;
	clear_depth_ = depth;
	clear_stencil_ = stencil;
}

void ClearCommand::reset()
{
	executed_ = false;
}

bool ClearCommandSimple::execute_impl(RenderStage /*current_stage*/)
{
	ASSERT(driver_ != nullptr, "You must setup the driver first");
    driver_->clear(true, true, true, vec4(0.0, 0.0, 0.0, 1.0));
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

GPUProfileCommand::GPUProfileCommand(const char* name) :
	node_(MainGPUProfiler::instance().create(name))
{
	set_stages(render_stage_before_submit | render_stage_after_submit);
}

bool GPUProfileCommand::execute_impl(RenderStage current_stage)
{
	uint8_t active_stages = stages();
	uint8_t begin_stage = render_stage_before_submit;
	uint8_t end_stage = render_stage_after_submit;
	if (active_stages & render_stage_begin_priority)
	{
		ASSERT(active_stages & render_stage_end_priority, "Invalid command flags. You must use either before/after_submit or begin/end_priority");
		begin_stage = render_stage_begin_priority;
		end_stage = render_stage_end_priority;
	}

	if (current_stage == begin_stage)
	{
		MainGPUProfiler::instance().begin(node_);
		node_.begin();
	}
	else if (current_stage == end_stage)
	{
		node_.end();
		MainGPUProfiler::instance().end(node_);
	}

	return true;
}
}
