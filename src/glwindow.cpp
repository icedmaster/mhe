// This module will be deleted

#include "glwindow.hpp"
#include "globals.hpp"

using namespace mhe;

GLWindow::GLWindow()
{
    width_ = default_width;
    height_ = default_height;
    bpp_ = default_bpp;
    is_fullscreen = false;
    create_window();
}


GLWindow::GLWindow(cmn::uint w, cmn::uint h, cmn::uint bpp, const std::string& name, bool fullscreen)
{
    width_ = w;
    height_ = h;
    bpp_ = bpp;
    name_ = name;
    is_fullscreen = fullscreen;
    create_window();
}


GLWindow::~GLWindow()
{
}


void GLWindow::resize(cmn::uint w, cmn::uint h)
{
    width_ = w;
    height_ = h;
    create_window();
}


void GLWindow::set_width(cmn::uint w)
{
    width_ = w;
    create_window();
}


void GLWindow::set_height(cmn::uint h)
{
    height_ = h;
    create_window();
}


void GLWindow::set_bpp(cmn::uint bpp)
{
    bpp_ = bpp;
    create_window();
}


void GLWindow::set_fullscreen(bool fullscreen)
{
    is_fullscreen = fullscreen;
    create_window();
}


void GLWindow::set_name(const std::string& name)
{
    name_ = name;
    SDL_WM_SetCaption(name.c_str(), NULL);
}


bool GLWindow::create_window()
{
	if (!(SDL_WasInit(0) & SDL_INIT_VIDEO))
	{
		SDL_InitSubSystem(SDL_INIT_VIDEO);
	}
	    cmn::uint flags = SDL_OPENGL;
    if (is_fullscreen)
        flags |= SDL_FULLSCREEN;
	srf = SDL_SetVideoMode(width_, height_, bpp_, flags);
    if (srf == NULL)
        return false;

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, bpp_);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_WM_SetCaption(name_.c_str(), NULL);

    globals::instance().set_window(width_, height_);
    return true;
}
