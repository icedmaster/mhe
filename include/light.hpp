#ifndef _LIGHT_HPP_
#define _LIGHT_HPP_

#include "ilight.hpp"
#include "types.hpp"
#include "mhe_gl.hpp"

namespace mhe
{
	class Light : public iLight
	{
		private:
			v4d pos;
			mutable v3d pos3;
			v3d dir;
			colorf diffuse_color;
			int gl_num;
			bool is_enable_;
			LightType lt;

			int get_id() const
			{
				return gl_num;
			}

			void set_id_impl(int id)
			{
				gl_num = id;
			}

			void set_position(const v3d& p)
			{
				float w = (lt == LightDirected) ? 1.0 : 0.0;
				pos.set(p, w);
			}

			const v3d& get_position() const
			{
                // TODO:
                pos.as_v3d(pos3);
				return pos3;
			}

			void set_direction(const v3d& d)
			{
				dir = d;
			}

			const v3d& get_direction() const
			{
				return dir;
			}

			void set_diffuse(const colorf& dc)
			{
				diffuse_color = dc;
			}

			const colorf& get_diffuse() const
			{
				return diffuse_color;
			}

			void set_ambient(const colorf& ac)
			{
			}

			const colorf& get_ambient() const
			{
				throw mhe::exception("Not implemented");
			}

			void set_light_type(LightType t)
			{
				lt = t;
				float w = (lt == LightDirected) ? 1.0 : 0.0;
				pos.set_w(w);
			}

			LightType get_light_type() const
			{
				return lt;
			}

			void set_enable(bool enable = true)
			{
				is_enable_ = enable;
				if (is_enable_)
                    glEnable(GL_LIGHT0 + gl_num);
                else
                    glDisable(GL_LIGHT0 + gl_num);
			}

			bool get_enable() const
			{
				return is_enable_;
			}

			void update_impl();

		public:
			Light();
			Light(int num, LightType t = LightDirected);
			Light(const Light& l);

	};

	inline void setGlobalAmbient(const colorf& a)
	{
	    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, a.get());
	}
};

#endif
