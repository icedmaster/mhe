#ifndef _TEXTURE_HPP_
#define _TEXTURE_HPP_

#include "itexture.hpp"
#include "geom.hpp"
#include "mhe_gl.hpp"

namespace mhe
{
	class Texture : public iTexture
	{
		private:
			cmn::uint id_;
			std::string name_;
			FilterType ft_;
			v2d c;	// texture coordinates
			boost::shared_ptr<Image> im_;
			StoreType st_;
			bool binded_;

			void rebuild_texture();
		private:
			// pure virtual functions implementation
			void set_image(const boost::shared_ptr<Image>& im, StoreType st, FilterType ft)
			{
				st_ = st;
				im_ = im;
				ft_ = ft;
				rebuild_texture();
			}

			void clear_impl()
			{
				if (binded_)
				{
					glDeleteTextures(1, &id_);
					if (st_ == Store)
						im_.reset();
				}
				binded_ = false;
			}

			void set_u_impl(float u)
			{
				c.set_x(u);
			}

			void set_v_impl(float v)
			{
				c.set_y(v);
			}

			float get_u() const
			{
				return c.x();
			}

			float get_v() const
			{
				return c.y();
			}

			void set_filter(FilterType ft)
			{
				ft_ = ft;
				if (st_ == Store)
					rebuild_texture();
				// else throw mhe::exception("");
			}

			cmn::uint get_id() const
			{
				return id_;
			}

			void set_name(const std::string& name)
			{
				name_ = name;
			}

			const std::string& get_name() const
			{
				return name_;
			}

		public:
			Texture() :
				id_(0),
				st_(NotStore),
				binded_(false)
			{}

			Texture(const Texture& t) :
				id_(t.id_),
				name_(t.name_),
				ft_(t.ft_),
				c(t.c),
				im_(t.im_),
				st_(t.st_),
				binded_(t.binded_)
			{}

			~Texture()
			{
				// delete texture from videobuffer
				if (binded_)
					glDeleteTextures(1, &id_);
			}
	};

};

#endif
