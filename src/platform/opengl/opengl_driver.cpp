#include "platform/opengl/opengl_driver.hpp"

namespace mhe {
namespace opengl {

	OpenGLDriver::OpenGLDriver() :
		zbuffer_masked_(false)
	{	
	}

	bool OpenGLDriver::init_impl()
	{
		OpenGLExtensions::instance().init_extensions();
		glEnable(GL_TEXTURE_2D);
		return true;
	}

	void OpenGLDriver::close_impl()
	{
	}

	void OpenGLDriver::save_view_matrix()
	{
	    glPushMatrix();
	}

	void OpenGLDriver::restore_view_matrix()
	{
	    glPopMatrix();
	}

	void OpenGLDriver::setup_view_matrix(const matrixf& m)
	{
	    glMultMatrixf(m.get());
	}

	void OpenGLDriver::load_projection_matrix(const matrixf& m)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(m.get());
	}

	void OpenGLDriver::load_modelview_matrix(const matrixf& m)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(m.get());
	}

	void OpenGLDriver::enable_lighting_impl()
	{
		GLStateAttr::instance().set(GL_LIGHTING);
	}

	void OpenGLDriver::disable_lighting_impl()
	{
		GLStateAttr::instance().clr(GL_LIGHTING);
	}

	void OpenGLDriver::enable_blending_impl()
	{
		GLStateAttr::instance().set(GL_BLEND);
	}

	void OpenGLDriver::disable_blending_impl()
	{
		GLStateAttr::instance().clr(GL_BLEND);
	}

	void OpenGLDriver::set_blend_func(BlendMode bf)
	{
	    switch (bf)
	    {
	        case alpha_one_minus_alpha:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;

            default: break;
	    }
	}

	void OpenGLDriver::enable_depth_impl()
	{
        GLStateAttr::instance().set(GL_DEPTH_TEST);
	}

	void OpenGLDriver::disable_depth_impl()
	{
        GLStateAttr::instance().clr(GL_DEPTH_TEST);
	}

    void OpenGLDriver::set_depth_func(DepthFunc)
    {
    }

    void OpenGLDriver::clear_depth_impl()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLDriver::clear_color_impl()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void OpenGLDriver::mask_zbuffer_impl()
    {
		if (zbuffer_masked_) return;
        glDepthMask(GL_FALSE);
		zbuffer_masked_ = true;
    }

	void OpenGLDriver::unmask_zbuffer_impl()
	{
		if (!zbuffer_masked_) return;
	    glDepthMask(GL_TRUE);
		zbuffer_masked_ = false;
	}

	void OpenGLDriver::set_clear_color_impl(const colorf& color)
	{
	    glClearColor(color.r(), color.g(), color.b(), color.a());
	}

	void OpenGLDriver::save_current_color()
	{
        glGetFloatv(GL_CURRENT_COLOR, cur_color);
	}

	void OpenGLDriver::restore_color()
	{
	    glColor4fv(cur_color);
	}

    void OpenGLDriver::begin_draw_impl(const float* v, const float* n,
                                       const float* t, const float* c,
                                       cmn::uint/* size*/)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        // 3 координаты на вершину, 0 - расстояние между тройками координат
        glVertexPointer(3, GL_FLOAT, 0, v);
		if (n)
			glNormalPointer(GL_FLOAT, 0, n);
        // need to change to nullptr
        if (t)
        {
            //glEnable(GL_TEXTURE_2D);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, 0, t);
            using_texture_ = true;
        }
        if (c)
        {
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(4, GL_FLOAT, 0, c);
            using_color_ = true;
        }
    }

	void OpenGLDriver::begin_draw_impl(boost::shared_ptr<Texture> texture,
									   const float* v, const float* n, const float* t, const float* c,
									   cmn::uint size)
	{
		texture->prepare();
		begin_draw_impl(v, n, t, c, size);
	}

    void OpenGLDriver::draw_impl(const cmn::uint* i, cmn::uint size)
    {
        glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, i);
    }

    void OpenGLDriver::end_draw_impl()
    {
        if (using_color_)
            glDisableClientState(GL_COLOR_ARRAY);
        if (using_texture_)
        {
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            //glDisable(GL_TEXTURE_2D);
        }
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }

	void OpenGLDriver::end_draw_impl(boost::shared_ptr<Texture> texture)
	{
        if (using_color_)
            glDisableClientState(GL_COLOR_ARRAY);
		texture->clean();		
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
	}

    void OpenGLDriver::set_color_impl(const colorf& c)
    {
        glColor4fv(c.get());
    }

    void OpenGLDriver::get_display_data_impl(std::vector<char>& data)
    {
		data.resize(ws_->width() * ws_->height() * 4);
        glReadPixels(0, 0, ws_->width(), ws_->height(), GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
    }

	void OpenGLDriver::set_viewport_impl(int x, int y, int w, int h)
	{
		glViewport(x, y, w, h);
		vp.set(x, y, w, h);
	}
}}
