#ifndef _MATERIAL_HPP_
#define _MATERIAL_HPP_

#include "imaterial.hpp"

namespace mhe
{
	class Material : public iMaterial
	{
		private:
			bool diffuse_front_and_back;
			colorf diffuse_color_front;
			colorf diffuse_color_back;
			bool ambient_front_and_back;
			colorf ambient_color_front;
			colorf ambient_color_back;

			void set_diffuse_front(const colorf& c)
			{
				diffuse_front_and_back = false;
				diffuse_color_front = c;
			}

			const colorf& get_diffuse_front() const
			{
				return diffuse_color_front;
			}

			void set_diffuse_back(const colorf& c)
			{
				diffuse_front_and_back = false;
				diffuse_color_back = c;
			}

			const colorf& get_diffuse_back() const
			{
				return diffuse_color_back;
			}

			void set_diffuse_front_and_back(const colorf& c)
			{
				diffuse_front_and_back = true;
				diffuse_color_front = c;
				diffuse_color_back = c;
			}

			void set_ambient_front(const colorf& c)
			{
				ambient_front_and_back = false;
				ambient_color_front = c;
			}

			const colorf& get_ambient_front() const
			{
				return ambient_color_front;
			}

			void set_ambient_back(const colorf& c)
			{
				ambient_front_and_back = false;
				ambient_color_back = c;
			}

			const colorf& get_ambient_back() const
			{
				return ambient_color_back;
			}

			void set_ambient_front_and_back(const colorf& c)
			{
				ambient_front_and_back = true;
				ambient_color_front = c;
				ambient_color_back = c;
			}

			void update_impl();
		public:
			Material();
			Material(const Material&);
	};
};

#endif
