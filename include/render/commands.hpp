#ifndef __COMMANDS_HPP__
#define __COMMANDS_HPP__

#include "node.hpp"

namespace mhe {

class Driver;
class RenderTarget;
class Texture;
class GPUProfilerNode;

class MHE_EXPORT ClearCommand : public RenderCommand
{
public:
    ClearCommand();

    void set_clear_mask(bool color, bool depth, bool stencil);

    void reset();
private:
    bool execute_impl(Context& context, RenderStage current_stage) override;
    bool executed_;
    bool clear_color_;
    bool clear_depth_;
    bool clear_stencil_;
};

class MHE_EXPORT ClearCommandSimple : public RenderCommand
{
private:
    bool execute_impl(Context& context, RenderStage current_stage) override;
};

class MHE_EXPORT SetRenderTargetTextureSideCommand : public RenderCommand
{
public:
    void setup(RenderTarget* render_target, int side)
    {
        render_target_ = render_target;
        side_ = side;
        executed_ = false;
    }
private:
    bool execute_impl(Context& context, RenderStage current_stage) override;

    RenderTarget* render_target_;
    int side_;
    bool executed_;
};

class MHE_EXPORT CopyFramebufferCommand : public RenderCommand
{
public:
    CopyFramebufferCommand();

    void set_texture(Texture* texture)
    {
        texture_ = texture;
    }
private:
    bool execute_impl(Context& context, RenderStage current_stage) override;

    Texture* texture_;
};

class MHE_EXPORT ListOfCommands : public RenderCommand
{
public:
    void add_command(RenderCommand* command)
    {
        commands_.push_back(command);
        set_stages(stages() | command->stages());
    }

    void clear()
    {
        commands_.clear();
    }

    bool empty() const
    {
        return commands_.empty();
    }
private:
    bool execute_impl(Context& context, RenderStage current_stage) override;

    fixed_capacity_vector<RenderCommand*, 4> commands_;
};

class MHE_EXPORT GPUProfileCommand : public RenderCommand
{
public:
    GPUProfileCommand(const char* name);
private:
    bool execute_impl(Context& context, RenderStage current_stage) override;

    GPUProfilerNode& node_;
};

}

#endif
