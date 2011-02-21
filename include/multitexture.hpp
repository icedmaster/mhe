#ifndef _MULTITEXTURE_HPP_
#define _MULTITEXTURE_HPP_

#include "texture.hpp"

namespace mhe
{
    class MultiTexture : public iTexture
    {
        private:
            std::string name_;
            std::vector< boost::shared_ptr<iTexture> > tex;
            v2d c[4];
        private:
            void set_image(const boost::shared_ptr<Image>& im, StoreType st, FilterType ft)
            {
                boost::shared_ptr<iTexture> t(new Texture);
                t->setImage(im, st, ft);
                tex.push_back(t);
            }

			void clear_impl()
			{
			    tex.clear();
			}

			void set_filter(FilterType ft)
			{
			    for (size_t i = 0; i < tex.size(); ++i)
                    tex[i]->setFilter(ft);
			}

			cmn::uint get_id() const
			{
			    return 0;   // not implemented for multitexture
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
                for (size_t i = 0; i < tex.size(); ++i)
                {
                    glActiveTexture(GL_TEXTURE0_ARB + i);
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, tex[i]->id());
                }
			}

			void clean_impl()
			{
			    for (size_t i = 0; i < tex.size(); ++i)
			    {
			        glActiveTexture(GL_TEXTURE0_ARB + i);
			        glDisable(GL_TEXTURE_2D);
			    }
			}

			void draw_impl(cmn::uint pos)
			{
			    for (size_t i = 0; i < tex.size(); ++i)
                    glMultiTexCoord2f(GL_TEXTURE0_ARB + i, c[pos].x(), c[pos].y());
			}

			void set_coord(cmn::uint pos, const v2d& coord)
			{
			    c[pos] = coord;
			}

			const v2d& get_coord(cmn::uint pos) const
			{
			    return c[pos];
			}

			cmn::uint get_width() const
			{
			    return tex[0]->width();
			}

			cmn::uint get_height() const
			{
			    return tex[0]->height();
			}

        public:
            MultiTexture()
            {
            }
    };
};

#endif
