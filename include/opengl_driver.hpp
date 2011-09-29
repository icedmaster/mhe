#ifndef _OPENGL_DRIVER_HPP_
#define _OPENGL_DRIVER_HPP_

#include "idriver.hpp"
#include "gl_state_attr.hpp"
#include "mhe_gl.hpp"
#include "gl_extension.hpp"

namespace mhe
{
	class OpenGLDriver : public iDriver
	{
		private:
            float cur_color[4];
            bool using_texture_;
            bool using_color_;
			WindowSystem* ws_;
			rect<int> vp;	// viewport

			// state
			bool zbuffer_masked_;
			BlendFunc bf_;

		OpenGLExtensions extensions_;
		private:
			bool init_impl();
			void close_impl();

			void set_ws(WindowSystem* ws)
			{
				ws_ = ws;
			}

			void save_view_matrix();
			void restore_view_matrix();
			void setup_view_matrix(const matrixf& m);

			void load_projection_matrix(const matrixf& m);
			void load_modelview_matrix(const matrixf& m);

			void enable_lighting_impl();
			void disable_lighting_impl();
			void enable_blending_impl();
			void disable_blending_impl();
			void set_blend_func(BlendFunc bf);

			void enable_depth_impl();
			void disable_depth_impl();
			void set_depth_func(DepthFunc);

			void clear_depth_impl();
			void clear_color_impl();

			void mask_zbuffer_impl();
			void unmask_zbuffer_impl();

			void set_clear_color_impl(const colorf& color);

			void save_current_color();
			void restore_color();
            void begin_draw_impl(const float*, const float*, const float*, const float*,
                                 cmn::uint);
            void draw_impl(const cmn::uint*, cmn::uint);
            void end_draw_impl();

            void set_color_impl(const colorf&);

            void get_display_data_impl(std::vector<char>& data);

			void set_viewport_impl(int x, int y, int w, int h);

		public:
			OpenGLDriver();

		// specific functions
		OpenGLExtensions& get_extensions()
		{
			return extensions_;
		}

	};
};

#endif
