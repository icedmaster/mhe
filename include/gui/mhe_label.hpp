#ifndef _MHE_LABEL_HPP_
#define _MHE_LABEL_HPP_

#include "gui/widget_impl.hpp"
#include <boost/shared_ptr.hpp>

namespace mhe
{
    namespace gui
    {
        class mheLabel : public LabelImpl
        {
            private:
                rect g;
                boost::shared_ptr<iFont> fnt;
                std::string caption_;
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
        };
    };
};

#endif
