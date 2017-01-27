#include "platform/opengl3/opengl3_render_state.hpp"
#include "platform/opengl3/opengl3_driver.hpp"

#include "platform/opengl/opengl_types.hpp"
#include "platform/opengl/opengl_extension.hpp"
#include "platform/opengl/opengl_helpers.hpp"

namespace mhe {
namespace opengl {

void DepthState::init(const DepthDesc& desc)
{
    desc_.test_enabled = desc.test_enabled;
    desc_.write_enabled = desc.write_enabled;
}

void DepthState::enable(OpenGL3ContextState& state) const
{
    if (desc_.test_enabled != state.depth_test)
    {
        if (desc_.test_enabled)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
        state.depth_test = desc_.test_enabled;
    }

    if (desc_.write_enabled != state.depth_write)
    {
        if (desc_.write_enabled)
            glDepthMask(GL_TRUE);
        else glDepthMask(GL_FALSE);
        state.depth_write = desc_.write_enabled;
    }
    CHECK_GL_ERRORS();
}

void StencilState::init(const StencilDesc& desc)
{
    desc_.enabled = desc.enabled;
    desc_.front_op_pass = get_stencil_op(desc.front_operation_pass);
    desc_.front_op_dfail = get_stencil_op(desc.front_operation_dfail);
    desc_.front_op_sfail = get_stencil_op(desc.front_operation_sfail);
    desc_.front_compare_mode = get_compare_mode(desc.front_compare_func);
    desc_.front_ref = desc.front_reference;
    desc_.front_mask = desc_.front_mask;
}

void StencilState::enable(OpenGL3ContextState& state) const
{
    if (state.stencil == desc_.enabled) return;
    if (desc_.enabled)
    {
        glEnable(GL_STENCIL_TEST);
        OpenGLExtensions::instance().glStencilFuncSeparate(GL_FRONT, desc_.front_compare_mode, desc_.front_ref, desc_.front_mask);
        OpenGLExtensions::instance().glStencilOpSeparate(GL_FRONT, desc_.front_op_sfail, desc_.front_op_dfail, desc_.front_op_pass);
    }
    else glDisable(GL_STENCIL_TEST);
    state.stencil = desc_.enabled;
    CHECK_GL_ERRORS();
}

void BlendState::init(const BlendDesc& desc)
{
    desc_.enabled = desc.enabled;
    GLboolean write_color = desc.color_write ? GL_TRUE : GL_FALSE;
    desc_.color_mask[0] = write_color;
    desc_.color_mask[1] = write_color;
    desc_.color_mask[2] = write_color;
    desc_.color_mask[3] = write_color;
    if (desc_.enabled)
    {
        desc_.func = get_blend_operation(desc.func);
        desc_.func_alpha = get_blend_operation(desc.func_alpha);
        desc_.srcmode = get_blend_mode(desc.srcmode);
        desc_.dstmode = get_blend_mode(desc.dstmode);
        desc_.srcmode_alpha = get_blend_mode(desc.srcmode_alpha);
        desc_.dstmode_alpha = get_blend_mode(desc.dstmode_alpha);
    }
}

void BlendState::enable(OpenGL3ContextState& state) const
{
    uint8_t color_mask = desc_.color_mask[0] | desc_.color_mask[1] << 1 | desc_.color_mask[2] << 2 | desc_.color_mask[3] << 3;
    if (color_mask != state.color_mask)
    {
        glColorMask(desc_.color_mask[0], desc_.color_mask[1], desc_.color_mask[2], desc_.color_mask[3]);
        state.color_mask = color_mask;
        CHECK_GL_ERRORS();
    }
    if (state.blend == desc_.enabled) return;
    if (desc_.enabled)
    {
        glEnable(GL_BLEND);
        OpenGLExtensions::instance().glBlendFuncSeparate(desc_.srcmode, desc_.dstmode, desc_.srcmode_alpha, desc_.dstmode_alpha);
        OpenGLExtensions::instance().glBlendEquation(desc_.func);
    }
    else glDisable(GL_BLEND);
    state.blend = desc_.enabled;
    CHECK_GL_ERRORS();
}

void RasterizerState::init(const RasterizerDesc& desc)
{
    desc_.cull = get_cull_mode(desc.cull);
    desc_.winding = get_winding_order(desc.order);
    desc_.disable_depth_test = !desc.depth_test_enabled;
}

void RasterizerState::enable(OpenGL3ContextState& state) const
{
    bool enable = desc_.cull != GL_NONE;
    bool enabled_changed = enable != state.cull;
    bool winding_changed = desc_.winding != state.cull_winding;
    bool mode_changed = desc_.cull != state.cull_mode;
    bool depth_test_mode_changed = desc_.disable_depth_test != state.rasterizer_depth_test_disabled;
    bool changed = enabled_changed || winding_changed || mode_changed || depth_test_mode_changed;
    if (changed)
    {
        if (!enable && enabled_changed)
            glDisable(GL_CULL_FACE);
        else if (enable)
        {
            if (enabled_changed)
                glEnable(GL_CULL_FACE);
            if (winding_changed)
                glFrontFace(desc_.winding);
            if (mode_changed)
                glCullFace(desc_.cull);
        }
        state.cull = enable;
        state.cull_winding = desc_.winding;
        state.cull_mode = desc_.cull;
        if (depth_test_mode_changed)
        {
            if (desc_.disable_depth_test)
                glEnable(GL_DEPTH_CLAMP);
            else
                glDisable(GL_DEPTH_CLAMP);
        }
        state.rasterizer_depth_test_disabled = desc_.disable_depth_test;
        CHECK_GL_ERRORS();
    }
}

bool OpenGL3RenderState::init(const RenderStateDesc& desc)
{
    depth_state_.init(desc.depth);
    stencil_state_.init(desc.stencil);
    blend_state_.init(desc.blend);
    rasterizer_state_.init(desc.rasterizer);
    viewport_ = desc.viewport.viewport;
    scissor_state_ = desc.scissor;
    return true;
}

void OpenGL3RenderState::close()
{}

void OpenGL3RenderState::update(const RenderStateDesc& desc)
{
    init(desc);
}

void OpenGL3RenderState::update_viewport(const ViewportDesc& viewport_desc)
{
    viewport_ = viewport_desc.viewport;
}

void OpenGL3RenderState::update_scissor(const ScissorDesc& scissor_desc)
{
    scissor_state_ = scissor_desc;
}

void OpenGL3RenderState::update_rasterizer(const RasterizerDesc& rasterizer_desc)
{
    rasterizer_state_.init(rasterizer_desc);
}

void OpenGL3RenderState::enable(OpenGL3ContextState& state) const
{
    depth_state_.enable(state);
    stencil_state_.enable(state);
    blend_state_.enable(state);
    rasterizer_state_.enable(state);
    if (viewport_.is_empty())
        set_viewport(state, rect<int>(0, 0, state.window_size.x(), state.window_size.y()));
    else
        set_viewport(state, viewport_);
    set_scissor(state, scissor_state_);
}

void OpenGL3RenderState::disable() const
{}

void OpenGL3RenderState::set_viewport(OpenGL3ContextState& state, const rect<int>& viewport) const
{
    if (state.viewport == viewport)
        return;
    glViewport(viewport.ll().x(), viewport.ll().y(), viewport.width(), viewport.height());
    state.viewport = viewport;
}

void OpenGL3RenderState::set_scissor(OpenGL3ContextState& state, const ScissorDesc& scissor_desc) const
{
    if (state.scissor_test == scissor_desc.enabled && state.scissor == scissor_desc.scissor_rect)
        return;
    if (scissor_desc.enabled)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
    CHECK_GL_ERRORS();
    glScissor(scissor_desc.scissor_rect.ll().x(), scissor_desc.scissor_rect.ll().y(),
        scissor_desc.scissor_rect.width(), scissor_desc.scissor_rect.height());
    CHECK_GL_ERRORS();
    state.scissor = scissor_desc.scissor_rect;
    state.scissor_test = scissor_desc.enabled;
}

}}
