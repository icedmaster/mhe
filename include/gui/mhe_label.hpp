#ifndef _MHE_LABEL_HPP_
#define _MHE_LABEL_HPP_

#include "gui/widget_impl.hpp"
#include "texture.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

namespace mhe
{
    namespace gui
    {
        class mheLabel : public LabelImpl
        {
            private:
                rect g;
                boost::shared_ptr<iFont> fnt;
                boost::scoped_ptr<iTexture> texture_;
                std::string caption_;
                colorf color_;
            private:
                void draw_impl();
                void set_geom(const rect& r)
                {
                    g = r;
                }

                const rect& get_geom() const
                {
                    return g;
                }

                void set_font(const boost::shared_ptr<iFont>& f)
                {
                    fnt = f;
                }

                void set_caption(const std::string& caption)
                {
                    caption_ = caption;
                }

                void set_background(const colorf& color)
                {
                    color_ = color;
                }

                void set_image(const std::string& filename);
        };
    };
};

#endif
