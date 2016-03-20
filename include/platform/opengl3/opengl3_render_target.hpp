#ifndef __OPENGL3_RENDER_TARGET_HPP__
#define __OPENGL3_RENDER_TARGET_HPP__

#include "render/render_target.hpp"
#include "../opengl/mhe_gl.hpp"
#include "opengl3_texture.hpp"

namespace mhe {
namespace opengl {

class OpenGL3RenderTarget : public RenderTargetImpl
{
public:
    bool init(const RenderTargetDesc& desc, Texture** color_textures, Texture* depth_texture);
    void close();

    void enable() const;
    void disable() const;

    void set_texture_side(int side);
private:
    GLenum target_;
    GLuint id_;
    GLuint color_textures_[max_simultaneous_render_targets_number];
    uint8_t side_offset_;
    uint8_t targets_number_;
    bool is_cubemap_;
    bool is_3d_;
};

}}

#endif
