#ifndef __VIDEO_DRIVER_HPP__
#define __VIDEO_DRIVER_HPP__

#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>
#include "types.hpp"
#include "mhe_math.hpp"
#include "window_system.hpp"
#include "renderable.hpp"
#include "shader_program.hpp"
#include "render_buffer.hpp"
#include "engine_config.hpp"
#include "fixed_size_vector.hpp"

namespace mhe
{

enum DepthFunc
{
};

class Texture;

class DriverImpl
{
public:
	virtual ~DriverImpl() {}

	virtual bool init() = 0;
	virtual void close() = 0;

	virtual void set_ws(WindowSystem*) {}

	virtual void save_view_matrix() {}
	virtual void restore_view_matrix() {}
	virtual void setup_view_matrix(const matrixf&) = 0;

	virtual void load_projection_matrix(const matrixf&) = 0;
	virtual void load_modelview_matrix(const matrixf&) = 0;

	virtual void enable_lighting() = 0;
	virtual void disable_lighting() = 0;
	virtual void enable_blending() = 0;
	virtual void disable_blending() = 0;
	virtual void set_blend_func(BlendMode) = 0;

	virtual void enable_depth() = 0;
	virtual void disable_depth() = 0;
	virtual void set_depth_func(DepthFunc) = 0;

	virtual void clear_depth() = 0;
	virtual void clear_color() = 0;

	virtual void mask_zbuffer() {}
	virtual void unmask_zbuffer() {}

	virtual void set_clear_color(const colorf&) {}

	virtual void save_current_color() {}
	virtual void restore_color() {}
	virtual void begin_draw(const float*, const float*, const float*, const float*,
							cmn::uint) = 0;
	virtual void begin_draw(boost::shared_ptr<Texture>,
							const float*, const float*, const float*, const float*,
							cmn::uint) = 0;
	virtual void begin_draw(const RenderBuffer*) {}
	virtual void draw(const cmn::uint*, cmn::uint) = 0;
	virtual void draw(const RenderBuffer*) {}
	virtual void end_draw() = 0;
	virtual void end_draw(boost::shared_ptr<Texture> texture) = 0;
	virtual void end_draw(const RenderBuffer*) {}

	virtual void set_color(const colorf&) {}

	virtual void get_display_data(std::vector<char>&) {}

	virtual void set_viewport(int x, int y, int w, int h) = 0;

	virtual void set_shader_program(const boost::shared_ptr<ShaderProgram>& program) = 0;
    
    virtual void begin_render() {}
    virtual void end_render() {}

	virtual bool support_buffered_render() const
	{
		return false;
	}

	// method for internal buffer using
	virtual RenderBuffer* create_render_buffer() const
	{
		return nullptr;
	}
};

class Driver
{
public:
	class Stats
	{
	public:
		Stats() : tris_(0), batches_(0)
		{}

		void reset()
		{
			tris_ = batches_ = frames_ = elements_count_ = 0;
		}

		void update(const Renderable& renderable);
		void update_frames();

		void add_renderable_count(cmn::uint count)
		{
			elements_count_ += count;
		}

		cmn::uint tris() const
		{
			return tris_;
		}

		cmn::uint batches() const
		{
			return batches_;
		}

		cmn::uint frames() const
		{
			return frames_;
		}

		cmn::uint elements_count() const
		{
			return elements_count_;
		}
	private:
		cmn::uint tris_;
		cmn::uint batches_;
		cmn::uint frames_;
		cmn::uint elements_count_;
	};
public:
	Driver();
	~Driver() {}

	void set_window_system(WindowSystem* ws)
	{
		impl_->set_ws(ws);
	}

	bool init()
	{
		return impl_->init();
	}

	void close()
	{
		impl_->close();
	}

	void enable_lighting()
	{
		impl_->enable_lighting();
	}

	void disable_lighting()
	{
		impl_->disable_lighting();
	}

	void enable_blending(BlendMode bf)
	{
		impl_->enable_blending();
		impl_->set_blend_func(bf);
	}

	void disable_blending()
	{
		impl_->disable_blending();
	}

	void enable_depth(DepthFunc df)
	{
		impl_->enable_depth();
		impl_->set_depth_func(df);
	}

	void disable_depth()
	{
		impl_->disable_depth();
	}

	void clear_depth()
	{
		impl_->clear_depth();
	}

	void clear_color()
	{
		impl_->clear_color();
	}

	void mask_zbuffer()
	{
		impl_->mask_zbuffer();
	}

	void unmask_zbuffer()
	{
		impl_->unmask_zbuffer();
	}

	void set_clear_color(const colorf& color)
	{
		impl_->set_clear_color(color);
	}

	// working with matrix
	void set_modelview_matrix(const matrixf& m)
	{
		impl_->load_modelview_matrix(m);
	}

	void set_projection_matrix(const matrixf& m)
	{
		impl_->load_projection_matrix(m);
	}

	void begin_render();
	void end_render();

	void begin_frame();
	void end_frame();

	void draw(const matrixf& m,
			  const float* v, const float* n, const float* t, const float* c,
			  const cmn::uint* i,
			  cmn::uint size
		)
	{
		impl_->save_view_matrix();
		impl_->setup_view_matrix(m);
		impl_->begin_draw(v, n, t, c, size);
		impl_->draw(i, size);
		impl_->end_draw();
		impl_->restore_view_matrix();
	}

	void draw(
		const float* v, const float* n, const float* t, const float* c,
		const cmn::uint* i,
		cmn::uint size
		)
	{
		impl_->begin_draw(v, n, t, c, size);
		impl_->draw(i, size);
		impl_->end_draw();
	}

	void draw(const matrixf& m, boost::shared_ptr<Texture> texture,
			  const float* v, const float* n, const float* t, const float* c,
			  const cmn::uint* i, cmn::uint size)
	{
		impl_->save_view_matrix();
		impl_->setup_view_matrix(m);
		impl_->begin_draw(texture, v, n, t, c, size);
		impl_->draw(i, size);
		impl_->end_draw(texture);
		impl_->restore_view_matrix();
	}

	void draw(Renderable* renderable);

	// Save display data to vector. Can be used for screenshots,
	// for example.
	void get_display_data(std::vector<char>& pb)
	{
		impl_->get_display_data(pb);
	}

	void set_viewport(int x, int y, int w, int h)
	{
		impl_->set_viewport(x, y, w, h);
	}

	void set_shader_program(const boost::shared_ptr<ShaderProgram>& program)
	{
		impl_->set_shader_program(program);
	}

	const Stats& stats() const
	{
		return stats_;
	}

	Stats& stats()
	{
		return stats_;
	}
private:
	typedef fixed_size_vector<Renderable*, initial_number_of_renderables> renderable_container;
	typedef fixed_size_vector<Renderable, initial_number_of_renderables> batched_container;
private:
	batched_container perform_batch() const;
	void perform_render(const Renderable& renderable);
	void set_render_flags(const Renderable& renderable);
	void clear_render_flags(const Renderable& renderable);
	void perform_buffered_render(const Renderable& Renderable);

	renderable_container renderable_elements_;
	Stats stats_;
	boost::scoped_ptr<DriverImpl> impl_;
};

}

#endif
