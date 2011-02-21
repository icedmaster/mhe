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
			boost::shared_ptr<Image> im_;
			StoreType st_;
			bool binded_;
			v2d c[4];
			std::vector<float> coord;
			cmn::uint w_, h_;

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

			void prepare_impl()
			{
			    glEnable(GL_TEXTURE_2D);
			    glBindTexture(GL_TEXTURE_2D, id_);
			    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			    glTexCoordPointer(2, GL_FLOAT, 0, &coord[0]);
			}

			void clean_impl()
			{
			    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			    glDisable(GL_TEXTURE_2D);
			}

			void draw_impl(cmn::uint pos)
			{
			    glTexCoord2f(c[pos].x(), c[pos].y());
			}

			void set_coord(cmn::uint pos, const v2d& coord)
			{
			    c[pos] = coord;
			}

			const v2d& get_coord(cmn::uint pos) const
			{
			    return c[pos];
			}

			void set_coord(const std::vector<float>& c)
			{
			    coord = c;

			}

			cmn::uint get_width() const
			{
			    return w_;
			}

			cmn::uint get_height() const
			{
			    return h_;
			}

		public:
			Texture() :
				id_(0),
				st_(NotStore),
				binded_(false),
				w_(0), h_(0)
			{}

			Texture(const Texture& t) :
				id_(t.id_),
				name_(t.name_),
				ft_(t.ft_),
				im_(t.im_),
				st_(t.st_),
				binded_(t.binded_),
				w_(0), h_(0)
			{}

			~Texture()
			{
				// delete texture from videobuffer
				if (binded_)
					glDeleteTextures(1, &id_);
			}

			Texture& operator= (const Texture& t)
			{
                id_ = t.id_;
				name_ = t.name_;
				ft_ = t.ft_;
				im_ = t.im_;
				st_ = t.st_;
				binded_ = t.binded_;
				for (int i = 0; i < 4; ++i)
                    c[i] = t.c[i];
				return *this;
			}
	};

};

#endif
