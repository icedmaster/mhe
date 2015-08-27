#ifndef __VIDEO_DRIVER_HPP__
#define __VIDEO_DRIVER_HPP__

#include <vector>
#include <list>
#include "mhe_math.hpp"
#include "core/types.hpp"
#include "core/ref_ptr.hpp"
#include "core/unique_ptr.hpp"
#include "material.hpp"
#include "render_common.hpp"

namespace mhe
{

struct Context;
struct NodeInstance;
struct RenderData;
struct Node;
struct DrawCall;
struct DrawCallExplicit;

class Texture;
class RenderState;
class ShaderProgram;
class RenderBuffer;
class IndexBuffer;
class UniformBuffer;
class Layout;
class RenderTarget;

class DriverImpl
{
public:
	struct DriverRenderingCapabilities
	{
		float max_anisotropic_level;
	};

	virtual ~DriverImpl() {}

	virtual bool init(DriverRenderingCapabilities& caps) = 0;
	virtual void close() = 0;

	virtual void set_window_size(const vector2<int>& size) = 0;

	virtual void enable_blending() = 0;
	virtual void disable_blending() = 0;

	virtual void enable_depth() = 0;
	virtual void disable_depth() = 0;

	virtual void clear_depth() = 0;
	virtual void clear_color() = 0;
	virtual void clear_stencil() = 0;

	virtual void set_clear_color(const colorf&) {}

	virtual void save_current_color() {}
	virtual void restore_color() {}

	virtual void set_viewport(int x, int y, int w, int h) = 0;

	virtual void set_state(const RenderState& state) = 0;
	virtual void set_shader_program(const ShaderProgram& program) = 0;
	virtual void set_vertex_buffer(const RenderBuffer& vbuffer) = 0;
	virtual void set_index_buffer(const IndexBuffer& ibuffer) = 0;
	virtual void set_uniform(const UniformBuffer& uniform, size_t unit) = 0;
	virtual void set_layout(const Layout& layout) = 0;
	virtual void set_texture(const Texture& texture, size_t unit) = 0;
	virtual void set_render_target(const RenderTarget& render_target) = 0;
	virtual void set_texture_buffer(const TextureBuffer& texture_buffer, size_t unit) = 0;
	virtual void set_default_render_target() = 0;
	virtual void draw(const RenderData& data) = 0;
	virtual void draw(size_t elements_number, size_t vbuffer_offset, size_t ibuffer_offset, size_t indices_number, Primitive primitive) = 0;

	virtual uint supported_versions(pair<uint, uint>* versions, uint size) const = 0;

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
		Stats() : tris_(0), batches_(0), frames_(0)
		{}

		void reset()
		{
			last_frames_ = frames_;
			last_tris_ = tris_;
			last_batches_ = batches_;
			tris_ = batches_ = frames_ = 0;
		}

		void update(size_t tris);
		void update_frames();

		uint tris() const
		{
			return last_tris_;
		}

		uint batches() const
		{
			return last_batches_;
		}

		uint frames() const
		{
			return last_frames_;
		}
	private:
		uint tris_;
		uint batches_;
		uint frames_;

		uint last_frames_;
		uint last_tris_;
		uint last_batches_;
	};

	struct State
	{
		size_t shader_program;
		size_t vertex_buffer;
		size_t index_buffer;
		size_t layout;
		size_t render_target;
		size_t state;
		size_t textures[material_textures_number];
		size_t uniforms[material_uniforms_number];
		size_t texture_buffers[material_texture_buffers_number];
	};
public:
	Driver();
	~Driver() {}

	bool init()
	{
		return impl_->init(caps_);
	}

	void close()
	{
		impl_->close();
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

	void clear(bool clear_color, bool clear_depth, bool clear_stencil,
		const colorf& color = color_black, float depth = 1.0f, uint stencil = 0);

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
		return versions_[current_version_].first;
	}

	uint minor_version_need() const
	{
		return versions_[current_version_].second;
	}

	bool next_version()
	{
		if (++current_version_ >= versions_number_) return false;
		return true;
	}

	/// Reset implementaion - need to call init()
	void reset();

	void begin_render();
	void end_render();

    void render(const Context& context, const DrawCall* draw_calls, size_t count);
	void render(const Context& context, const DrawCallExplicit* draw_calls, size_t count);

	void set_window_size(const vector2<int>& size)
	{
		impl_->set_window_size(size);
	}

	void reset_state();

	// capabilities
	float max_anisotropic_level() const
	{
		return caps_.max_anisotropic_level;
	}
private:
	void perform_draw_call(const Context& context, const DrawCall& draw_call);
	void perform_draw_call(const Context& context, const DrawCallExplicit& draw_call);

	pair<uint, uint> versions_[8];
	uint versions_number_;
	uint current_version_;
	Stats stats_;
	State state_;
	DriverImpl::DriverRenderingCapabilities caps_;
	unique_ptr<DriverImpl> impl_;
};

}

#endif
