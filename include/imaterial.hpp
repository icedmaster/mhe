#ifndef _IMATERIAL_HPP_
#define _IMATERIAL_HPP_

#include "geom.hpp"

namespace mhe
{
	enum Face
	{
		FRONT_FACE,
		BACK_FACE,
		FRONT_AND_BACK_FACE
	};
	// abstract class for material representation
	class iMaterial
	{
		private:
			virtual void set_diffuse_front(const colorf& c) = 0;
			virtual const colorf& get_diffuse_front() const = 0;
			virtual void set_diffuse_back(const colorf&) = 0;
			virtual const colorf& get_diffuse_back() const = 0;
			virtual void set_diffuse_front_and_back(const colorf&) = 0;

			virtual void update_impl() = 0;
		public:
			void setDiffuse(Face face, const colorf& c)
			{
				switch (face)
				{
					case FRONT_AND_BACK_FACE:
						set_diffuse_front_and_back(c);
					break;

					case FRONT_FACE:
						set_diffuse_front(c);
					break;

					case BACK_FACE:
						set_diffuse_back(c);
					break;

					default: break;
				}
			}

            /*
			void setAmbient(Face face, const colorf& c)
			{
			    switch (face)
				{
					case FRONT_AND_BACK_FACE:
						set_ambient_front_and_back(c);
					break;

					case FRONT_FACE:
						set_ambient_front(c);
					break;

					case BACK_FACE:
						set_ambient_back(c);
					break;

					default: break;
				}
			}

			const colorf& getDiffuse(Face face) const
			{
				switch (face)
				{
					case FRONT_AND_BACK_FACE:
					case FRONT_FACE:
						return get_diffuse_front();

					case BACK_FACE:
						return get_diffuse_back();

					default: break;
				}
			}*/

			void update()
			{
				update_impl();
			}
	};
};

#endif
