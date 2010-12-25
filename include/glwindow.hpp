#ifndef _GL_WINDOW_H_
#define _GL_WINDOW_H_

#include "types.hpp"
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

namespace mhe
{

    const cmn::uint default_width = 640;
    const cmn::uint default_height = 480;
    const cmn::uint default_bpp = 32;

class GLWindow
{
  private:
    cmn::uint width_;
    cmn::uint height_;
    cmn::uint bpp_;
    bool is_fullscreen;
    std::string name_;
    SDL_Surface *srf;

    bool create_window();
  public:
    GLWindow();
    GLWindow(cmn::uint w, cmn::uint h, cmn::uint bpp, const std::string& name = "Window", bool fullscreen = false);
    ~GLWindow();

    void resize(cmn::uint w, cmn::uint h);

    void set_width(cmn::uint w);

    inline cmn::uint width() const
    {
        return width_;
    }

    void set_height(cmn::uint h);

    inline cmn::uint height() const
    {
        return height_;
    }

    void set_bpp(uint bpp);

    inline uint bpp() const
    {
        return bpp_;
    }

    inline bool fullscreen() const
    {
        return is_fullscreen;
    }

    void set_fullscreen(bool fullscreen);

    void set_name(const std::string& name);

    inline std::string name() const
    {
        return name_;
    }
};

};

#endif

