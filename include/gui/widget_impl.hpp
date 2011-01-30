#ifndef _WIDGET_IMPL_HPP_
#define _WIDGET_IMPL_HPP_

#include "rect.hpp"
#include "gui/ifont.hpp"
#include <boost/shared_ptr.hpp>

namespace mhe
{
    namespace gui
    {
        class LabelImpl
        {
            private:
                virtual void draw_impl() = 0;
                virtual void set_geom(const rect&) = 0;
                virtual const rect& get_geom() const = 0;
                virtual void set_font(const boost::shared_ptr<iFont>&) = 0;
                virtual void set_caption(const std::string&) = 0;
                virtual void set_background(const colorf&) = 0;
                virtual void set_image(const std::string& fn) = 0;
            public:
                void draw()
                {
                    draw_impl();
                }

                void setGeometry(const rect& r)
                {
                    set_geom(r);
                }

                const rect& geom() const
                {
                    return get_geom();
                }

                void setFont(const boost::shared_ptr<iFont>& f)
                {
                    set_font(f);
                }

                void setCaption(const std::string& caption)
                {
                    set_caption(caption);
                }

                void setBackground(const colorf& color)
                {
                    set_background(color);
                }

                void setImage(const std::string& fn)
                {
                    set_image(fn);
                }
        };
    };
};

#endif
