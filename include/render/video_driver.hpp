#ifndef __VIDEO_DRIVER_HPP__
#define __VIDEO_DRIVER_HPP__

#include <vector>
#include <list>
#include "mhe_math.hpp"
#include "core/types.hpp"
#include "core/ref_ptr.hpp"
#include "core/unique_ptr.hpp"

namespace mhe
{

enum DepthFunc
{
};

enum BlendFunc
{
};

struct Context;
struct Node;
struct RenderData;

class Texture;
class RenderState;
class ShaderProgram;
class RenderBuffer;
class IndexBuffer;
class UniformBuffer;
class Layout;

class DriverImpl
{
public:
	virtual ~DriverImpl() {}

	virtual bool init() = 0;
	virtual void close() = 0;

	virtual void enable_blending() = 0;
	virtual void disable_blending() = 0;

	virtual void enable_depth() = 0;
	virtual void disable_depth() = 0;
	virtual void set_depth_func(DepthFunc) = 0;

	virtual void clear_depth() = 0;
	virtual void clear_color() = 0;

	virtual void set_clear_color(const colorf&) {}

	virtual void save_current_color() {}
	virtual void restore_color() {}

	virtual void set_viewport(int x, int y, int w, int h) = 0;

	virtual void set_state(const RenderState& state) = 0;
	virtual void set_shader_program(const ShaderProgram& program) = 0;
	virtual void set_vertex_buffer(const RenderBuffer& vbuffer) = 0;
	virtual void set_index_buffer(const IndexBuffer& ibuffer) = 0;
	virtual void set_uniform(const UniformBuffer& uniform) = 0;
	virtual void set_layout(const Layout& layout) = 0;
	virtual void set_texture(const Texture& texture, size_t unit) = 0;
	virtual void draw(const RenderData& data) = 0;
    
	virtual uint major_version_need() const = 0;
	virtual uint minor_version_need() const = 0;

	virtual void flush() = 0;
};

class MHE_EXPORT Driver
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

		void update();
		void update_frames();

		void add_renderable_count(uint count)
		{
			elements_count_ += count;
		}

		uint tris() const
		{
			return tris_;
		}

		uint batches() const
		{
			return batches_;
		}

		uint frames() const
		{
			return frames_;
		}

		uint elements_count() const
		{
			return elements_count_;
		}
	private:
		uint tris_;
		uint batches_;
		uint frames_;
		uint elements_count_;
	};
public:
	Driver();
	~Driver() {}

	bool init()
	{
		return impl_->init();
	}

	void close()
	{
		impl_->close();
	}

    void enable_blending(BlendFunc /*bf*/)
	{
		impl_->enable_blending();
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

	void set_clear_color(const colorf& color)
	{
		impl_->set_clear_color(color);
	}

	void set_viewport(int x, int y, int w, int h)
	{
		impl_->set_viewport(x, y, w, h);
	}

	const Stats& stats() const
	{
		return stats_;
	}

	Stats& stats()
	{
		return stats_;
	}

	uint major_version_need() const
	{
		return impl_->major_version_need();
	}

	uint minor_version_need() const
	{
		return impl_->minor_version_need();
	}

	/// Reset implementaion - need to call init()
	void reset();

	void begin_render()
	{}

	void end_render()
	{
		impl_->flush();
	}

	void render(const Context& context, const Node* nodes, size_t count);
private:
	Stats stats_;
	unique_ptr<DriverImpl> impl_;
};

}

#endif
