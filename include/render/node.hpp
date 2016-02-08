#ifndef __NODE_HPP__
#define __NODE_HPP__

#include <vector>
#include "mesh.hpp"

namespace mhe {

struct Context;

enum RenderStage
{
	render_stage_before_render_target_setup = 1,
	render_stage_before_submit = 1 << 1,
	render_stage_after_submit = 1 << 2,
	render_stage_begin_priority = 1 << 3,
    render_stage_end_priority = 1 << 4,
    render_stage_after_render_target_setup = 1 << 5
};

class RenderCommand
{
public:
	RenderCommand() : stages_(render_stage_before_submit) {}
	virtual ~RenderCommand() {}
	bool execute(Context& context, RenderStage current_stage)
	{
		ASSERT(stages_ & current_stage, "Trying to execute a command with invalid render stage");
		return execute_impl(context, current_stage);
	}

	uint8_t stages() const
	{
		return stages_;
	}

	void set_stages(uint8_t stages)
	{
		stages_ = stages;
	}

	void add_stage(uint8_t stage)
	{
		stages_ |= stage;
	}
private:
	virtual bool execute_impl(Context& context, RenderStage current_stage) = 0;

	uint8_t stages_;
};

struct Node
{
};

}

#endif
