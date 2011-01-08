#ifndef _ILIGHT_HPP_
#define _ILIGHT_HPP_

#include "geom.hpp"

namespace mhe
{
	enum LightType
	{
		LightDirected,
		LightDiffused
	};

	class iLight
	{
		protected:
			virtual int get_id() const = 0;
			virtual void set_id_impl(int id) = 0;
			virtual void set_position(const v3d&) = 0;
			virtual const v3d& get_position() const = 0;
			virtual void set_direction(const v3d&) = 0;
			virtual const v3d& get_direction() const = 0;
			virtual void set_diffuse(const colorf& dc) = 0;
			virtual const colorf& get_diffuse() const = 0;
			virtual void set_ambient(const colorf& ac) = 0;
			virtual const colorf& get_ambient() const = 0;
			virtual void set_light_type(LightType) = 0;
			virtual LightType get_light_type() const = 0;
			virtual void set_enable(bool enable = true) = 0;
			virtual bool get_enable() const = 0;
		private:
			virtual void update_impl() = 0;
		public:
			void set_id(int id)
			{
				set_id_impl(id);
			}

			int id() const
			{
				return get_id();
			}

			void setPosition(const v3d& p)
			{
				set_position(p);
			}

			const v3d& position() const
			{
				return get_position();
			}

			void setDirection(const v3d& d)
			{
				set_direction(d);
			}

			const v3d& direction() const
			{
				return get_direction();
			}
			// access methods
			void setDiffuse(const colorf& dc)
			{
				set_diffuse(dc);
			}

			const colorf& getDiffuse() const
			{
				return get_diffuse();
			}

			void setAmbient(const colorf& ac)
			{
				set_ambient(ac);
			}

			const colorf& getAmbient() const
			{
				return get_ambient();
			}

			void enable()
			{
				set_enable();
			}

			void disable()
			{
				set_enable(false);
			}

			bool is_enable() const
			{
				return get_enable();
			}

			void update()
			{
				update_impl();
			}

			void setLightType(LightType t)
			{
				set_light_type(t);
			}

			LightType getLightType() const
			{
				return get_light_type();
			}
	};

};

#endif

