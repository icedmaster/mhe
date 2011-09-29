#ifndef _IDRIVER_HPP_
#define _IDRIVER_HPP_

#include <vector>
#include <boost/shared_ptr.hpp>
#include "types.hpp"
#include "mhe_math.hpp"
//#include "geom.hpp"
//#include "iTexture.hpp"
#include "window_system.hpp"
//#include "viewport.hpp"

namespace mhe
{
enum BlendFunc
{
	ALPHA_ONE_MINUS_ALPHA
};

enum DepthFunc
{
};

class iDriver
{
private:
	virtual bool init_impl() = 0;
	virtual void close_impl() = 0;

	virtual void set_ws(WindowSystem*) {}

	virtual void save_view_matrix() {}
	virtual void restore_view_matrix() {}
	virtual void setup_view_matrix(const matrixf&) = 0;

	virtual void load_projection_matrix(const matrixf&) = 0;
	virtual void load_modelview_matrix(const matrixf&) = 0;

	virtual void enable_lighting_impl() = 0;
	virtual void disable_lighting_impl() = 0;
	virtual void enable_blending_impl() = 0;
	virtual void disable_blending_impl() = 0;
	virtual void set_blend_func(BlendFunc) = 0;

	virtual void enable_depth_impl() = 0;
	virtual void disable_depth_impl() = 0;
	virtual void set_depth_func(DepthFunc) = 0;

	virtual void clear_depth_impl() = 0;
	virtual void clear_color_impl() = 0;

	virtual void mask_zbuffer_impl() {}
	virtual void unmask_zbuffer_impl() {}

	virtual void set_clear_color_impl(const colorf&) {}

	virtual void save_current_color() {}
	virtual void restore_color() {}
	virtual void begin_draw_impl(const float*, const float*, const float*, const float*,
								 cmn::uint) = 0;
	virtual void draw_impl(const cmn::uint*, cmn::uint) = 0;
	virtual void end_draw_impl() = 0;

	virtual void set_color_impl(const colorf&) {}

	virtual void get_display_data_impl(std::vector<char>&) {}

	virtual void set_viewport_impl(int x, int y, int w, int h) = 0;
public:
	virtual ~iDriver() {}

	void set_window_system(WindowSystem* ws)
	{
		set_ws(ws);
	}

	bool init()
	{
		return init_impl();
	}

	void close()
	{
		close_impl();
	}

	void enable_lighting()
	{
		enable_lighting_impl();
	}

	void disable_lighting()
	{
		disable_lighting_impl();
	}

	void enable_blending(BlendFunc bf)
	{
		enable_blending_impl();
		set_blend_func(bf);
	}

	void disable_blending()
	{
		disable_blending_impl();
	}

	void enable_depth(DepthFunc df)
	{
		enable_depth_impl();
		set_depth_func(df);
	}

	void disable_depth()
	{
		disable_depth_impl();
	}

	void clear_depth()
	{
		clear_depth_impl();
	}

	void clear_color()
	{
		clear_color_impl();
	}

	void mask_zbuffer()
	{
		mask_zbuffer_impl();
	}

	void unmask_zbuffer()
	{
		unmask_zbuffer_impl();
	}

	void set_clear_color(const colorf& color)
	{
		set_clear_color_impl(color);
	}

	// working with matrix
	void set_modelview_matrix(const matrixf& m)
	{
		load_modelview_matrix(m);
	}

	void set_projection_matrix(const matrixf& m)
	{
		load_projection_matrix(m);
	}

	// different draw() methods
	// ---------- draw() with color ---------
	void draw(const std::vector<float>& v, const std::vector<float>& n,
			  const std::vector<cmn::uint>& i,
			  const colorf& c)
	{
		save_current_color();
		set_color_impl(c);
		// need to change to nullptr
		begin_draw_impl(&v[0], &n[0], 0, 0, v.size());
		draw_impl(&i[0], i.size());
		end_draw_impl();
		restore_color();
	}

	void draw(const float* v, const float* n, const cmn::uint* i,
			  cmn::uint size,
			  const colorf& c)
	{
		if (!size) return;
		save_current_color();
		set_color_impl(c);
		// need to change to nullptr
		begin_draw_impl(v, n, 0, 0, size);
		draw_impl(i, size);
		end_draw_impl();
		restore_color();
	}

	void draw(const matrixf& m,
			  const std::vector<float>& v, const std::vector<float>& n,
			  const std::vector<cmn::uint>& i,
			  const colorf& c)
	{
		save_view_matrix();
		setup_view_matrix(m);
		save_current_color();
		set_color_impl(c);
		// !!nullptr
		begin_draw_impl(&v[0], &n[0], 0, 0, v.size());
		draw_impl(&i[0], i.size());
		end_draw_impl();
		restore_color();
		restore_view_matrix();
	}

	void draw(const matrixf& m,
			  const float* v, const float* n, const float* t, const float* c,
			  const cmn::uint* i,
			  cmn::uint size
		)
	{
		save_view_matrix();
		setup_view_matrix(m);
		begin_draw_impl(v, n, t, c, size);
		draw_impl(i, size);
		end_draw_impl();
		restore_view_matrix();
	}

	void draw(
		const float* v, const float* n, const float* t, const float* c,
		const cmn::uint* i,
		cmn::uint size
		)
	{
		begin_draw_impl(v, n, t, c, size);
		draw_impl(i, size);
		end_draw_impl();
	}


	// Save display data to vector. Can be used for screenshots,
	// for example.
	void get_display_data(std::vector<char>& pb)
	{
		get_display_data_impl(pb);
	}

	void set_viewport(int x, int y, int w, int h)
	{
		set_viewport_impl(x, y, w, h);
	}
};
};

#endif
